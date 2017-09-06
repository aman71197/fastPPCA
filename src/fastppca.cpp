/** 
 All of this code is written by Aman Agrawal 
 (Indian Institute of Technology, Delhi)
*/

#include <bits/stdc++.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/LU>
#include <Eigen/SVD>
#include "genotype.h"
#include "mailman.h"
#include "helper.h"
#include "time.h"
#include <Eigen/QR>
#include "storage.h"
#include <fenv.h>


using namespace Eigen;
using namespace std;


struct timespec t0;
struct timespec
elapsed ()
{
  struct timespec ts;
  clock_gettime (CLOCK_REALTIME, &ts);
  if (ts.tv_nsec < t0.tv_nsec)
    {
      ts.tv_nsec = 1000000000 + ts.tv_nsec - t0.tv_nsec;
      ts.tv_sec--;
    }
  ts.tv_sec -= t0.tv_sec;
  return (ts);
}



int
timelog (const char* message)
{
  struct timespec ts = elapsed ();
  return (printf ("[%06ld.%09ld] %s\n", ts.tv_sec, ts.tv_nsec, message));
}



clock_t total_begin = clock();

int MAX_ITER;
genotype g;
int k,p,n;
int k_orig;

MatrixXd c; //(p,k)
MatrixXd x; //(k,n)
MatrixXd v; //(p,k)
MatrixXd means; //(p,1)

options command_line_opts;

bool debug = false;
bool check_accuracy = false;
bool var_normalize=false;
int accelerated_em=0;
double convergence_limit;
bool memory_efficient = false;
bool missing=false;

bool fast_mode = true;

void print_timenl () {
	clock_t c = clock();
	double t = double(c) / CLOCKS_PER_SEC;
	cout << "Time = " << t << endl ;	
}

void print_time () {
	clock_t c = clock();
	double t = double(c) / CLOCKS_PER_SEC;
	cout << "Time = " << t  << " : ";	
}

void multiply_y_pre_fast(MatrixXd &op, int Ncol_op ,MatrixXd &res,bool subtract_means){
	double *sum_op = new double[Ncol_op];
	double *yint_m = new double[(int)pow(3,g.segment_size_hori)];
	if(debug){
		print_time (); 
		cout <<"Starting mailman on premultiply"<<endl;
		cout << "Nops = " << Ncol_op << "\t" <<g.Nsegments_hori << endl;
		cout << "Segment size = " << g.segment_size_hori << endl;
		cout << "Matrix size = " <<g.segment_size_hori<<"\t" <<g.Nindv << endl;
	}

	for(int k_iter=0;k_iter<Ncol_op;k_iter++){
		sum_op[k_iter]=op.col(k_iter).sum();
	}
	for(int seg_iter=0;seg_iter<g.Nsegments_hori;seg_iter++){
		std::vector<int> &p_tmp = g.p[seg_iter];
		std::vector<unsigned int> &p_eff_tmp = g.p_eff[seg_iter];
		for(int k_iter=0;k_iter<Ncol_op;k_iter++){
			double *y = new double[g.segment_size_hori];
			if(seg_iter==g.Nsegments_hori-1){
				if(g.Nsnp%g.segment_size_hori!=0){
					double *y_final = new double[g.Nsnp%g.segment_size_hori];
					if(memory_efficient)
						mailman::fastmultiply3(g.Nsnp%g.segment_size_hori,g.Nindv,p_eff_tmp,op,yint_m,y_final,g.Nbits_hori,k_iter);
					else
						mailman::fastmultiply2(g.Nsnp%g.segment_size_hori,g.Nindv,p_tmp,op,yint_m,y_final,k_iter);
					for(int p_iter=seg_iter*g.segment_size_hori;p_iter<seg_iter*g.segment_size_hori + g.Nsnp%g.segment_size_hori  && p_iter<g.Nsnp;p_iter++)
						res(p_iter,k_iter) = y_final[p_iter-(seg_iter*g.segment_size_hori)];
				}
			}
			else{
				if(memory_efficient)
					mailman::fastmultiply3(g.segment_size_hori,g.Nindv,p_eff_tmp,op,yint_m,y,g.Nbits_hori,k_iter);
				else
					mailman::fastmultiply2(g.segment_size_hori,g.Nindv,p_tmp,op,yint_m,y,k_iter);
				int p_base = seg_iter*g.segment_size_hori; 
				for(int p_iter=p_base; (p_iter<p_base+g.segment_size_hori) && (p_iter<g.Nsnp) ; p_iter++ ) 
					res(p_iter,k_iter) = y[p_iter-p_base];
			}
		}
	}
	if(debug){
		print_time (); 
		cout <<"Ending mailman on premultiply"<<endl;
	}
	if(!subtract_means)
		return;
	for(int p_iter=0;p_iter<p;p_iter++){
		for(int k_iter=0;k_iter<Ncol_op;k_iter++){
			res(p_iter,k_iter) = res(p_iter,k_iter) - (g.get_col_mean(p_iter)*sum_op[k_iter]);
			if(var_normalize)
				res(p_iter,k_iter) = res(p_iter,k_iter)/(g.get_col_std(p_iter));
		}
	}	
}

void multiply_y_post_fast(MatrixXd &op_orig, int Nrows_op, MatrixXd &res,bool subtract_means){
	double *yint_e = new double[(int)pow(3,g.segment_size_ver)];
	MatrixXd op;
	op = op_orig.transpose();

	if(var_normalize){
		for(int p_iter=0;p_iter<p;p_iter++){
			for(int k_iter=0;k_iter<Nrows_op;k_iter++)
				op(p_iter,k_iter) = op(p_iter,k_iter) / (g.get_col_std(p_iter));
		}
	}

	if(debug){
		print_time (); cout <<"Starting mailman on postmultiply"<<endl;
		cout << "Nops = " << Nrows_op << "\t" <<g.Nsegments_ver << endl;
		cout << "Segment size = " << g.segment_size_ver << endl;
		cout << "Matrix size = " <<g.segment_size_ver <<"\t" <<g.Nsnp << endl;
	}

	int Ncol_op = Nrows_op;
	for(int seg_iter=0; seg_iter < g.Nsegments_ver; seg_iter++){
		vector<int> &q_tmp = g.q[seg_iter];
		vector<unsigned int> &q_eff_tmp = g.q_eff[seg_iter];
		for(int k_iter=0;k_iter<Ncol_op;k_iter++){
			double *y = new double[g.segment_size_ver];
			if(seg_iter==g.Nsegments_ver-1){
				if(g.Nindv%g.segment_size_ver!=0){
					double *y_final = new double[g.Nindv%g.segment_size_ver];
					if(memory_efficient)
						mailman::fastmultiply3(g.Nindv%g.segment_size_ver,g.Nsnp,q_eff_tmp,op,yint_e,y_final,g.Nbits_ver,k_iter);
					else
						mailman::fastmultiply2(g.Nindv%g.segment_size_ver,g.Nsnp,q_tmp,op,yint_e,y_final,k_iter);
					for(int n_iter=seg_iter*g.segment_size_ver ; n_iter<seg_iter*g.segment_size_ver + g.Nindv%g.segment_size_ver  && n_iter<g.Nindv ; n_iter++)
						res(k_iter,n_iter) = y_final[n_iter-(seg_iter*g.segment_size_ver)];
				}

			}
			else{
				if(memory_efficient)
					mailman::fastmultiply3(g.segment_size_ver,g.Nsnp,q_eff_tmp,op,yint_e,y,g.Nbits_ver,k_iter);
				else
					mailman::fastmultiply2(g.segment_size_ver,g.Nsnp,q_tmp,op,yint_e,y,k_iter);
				int n_base = seg_iter*g.segment_size_ver; 
				for(int n_iter=n_base; (n_iter<n_base+g.segment_size_ver) && (n_iter<g.Nindv) ; n_iter++ ) 
					res(k_iter,n_iter) = y[n_iter-n_base];
			}
		}
	}
	if(debug){
		print_time (); 
		cout <<"Ending mailman on postmultiply"<<endl;
	}

	if(!subtract_means)
		return;
	double *sums_elements = new double[Ncol_op];
	memset (sums_elements, 0, Nrows_op * sizeof(int));

	for(int k_iter=0;k_iter<Ncol_op;k_iter++){
		double sum_to_calc=0.0;
		for(int p_iter=0;p_iter<p;p_iter++)
			sum_to_calc += g.get_col_mean(p_iter)*op(p_iter,k_iter);
		sums_elements[k_iter] = sum_to_calc;
	}
	for(int k_iter=0;k_iter<Ncol_op;k_iter++){
		for(int n_iter=0;n_iter<n;n_iter++)
			res(k_iter,n_iter) = res(k_iter,n_iter) - sums_elements[k_iter];
	}
}

void multiply_y_pre_naive(MatrixXd &op, int Ncol_op ,MatrixXd &res){
	for(int p_iter=0;p_iter<p;p_iter++){
		for(int k_iter=0;k_iter<Ncol_op;k_iter++){
			double temp=0;
			for(int n_iter=0;n_iter<n;n_iter++)
				temp+= g.get_geno(p_iter,n_iter,var_normalize)*op(n_iter,k_iter);
			res(p_iter,k_iter)=temp;
		}
	}
}

void multiply_y_post_naive(MatrixXd &op, int Nrows_op ,MatrixXd &res){
	for(int n_iter=0;n_iter<n;n_iter++){
		for(int k_iter=0;k_iter<Nrows_op;k_iter++){
			double temp=0;
			for(int p_iter=0;p_iter<p;p_iter++)
				temp+= op(k_iter,p_iter)*(g.get_geno(p_iter,n_iter,var_normalize));
			res(k_iter,n_iter)=temp;
		}
	}
}

void multiply_y_post(MatrixXd &op, int Nrows_op ,MatrixXd &res,bool subtract_means){
    if(fast_mode)
        multiply_y_post_fast(op,Nrows_op,res,subtract_means);
    else
        multiply_y_post_naive(op,Nrows_op,res);
}

void multiply_y_pre(MatrixXd &op, int Ncol_op ,MatrixXd &res,bool subtract_means){
    if(fast_mode)
        multiply_y_pre_fast(op,Ncol_op,res,subtract_means);
    else
        multiply_y_pre_naive(op,Ncol_op,res);
}

pair<double,double> get_error_norm(MatrixXd &c){
	HouseholderQR<MatrixXd> qr(c);
	MatrixXd Q;
	Q = qr.householderQ() * MatrixXd::Identity(p,k);
	MatrixXd q_t(k,p);
	q_t = Q.transpose();
	MatrixXd b(k,n);
	multiply_y_post(q_t,k,b,true);
	JacobiSVD<MatrixXd> b_svd(b, ComputeThinU | ComputeThinV);
	MatrixXd u_l,d_l,v_l; 
	if(fast_mode)
        u_l = b_svd.matrixU();
    else
        u_l = Q * b_svd.matrixU();
	v_l = b_svd.matrixV();
	d_l = MatrixXd::Zero(k,k);
	for(int kk=0;kk<k; kk++)
		d_l(kk,kk) = (b_svd.singularValues())(kk);
	
	MatrixXd u_k,v_k,d_k;
	u_k = u_l.leftCols(k_orig);
	v_k = v_l.leftCols(k_orig);
	d_k = MatrixXd::Zero(k_orig,k_orig);
	for(int kk =0 ; kk < k_orig ; kk++)
		d_k(kk,kk)  =(b_svd.singularValues())(kk);

	MatrixXd b_l,b_k;
    b_l = u_l * d_l * (v_l.transpose());
    b_k = u_k * d_k * (v_k.transpose());

    if(fast_mode){
        double temp_k=0.0;
        double temp_l=0.0;
        for(int k_iter=0;k_iter<k;k_iter++){
            for(int n_iter=0;n_iter<n;n_iter++){
                temp_k += b_k(k_iter,n_iter)*b(k_iter,n_iter);
                temp_l += b_l(k_iter,n_iter)*b(k_iter,n_iter);
            }
        }
        double b_knorm = b_k.norm();
        double b_lnorm = b_l.norm();
        double norm_k = (b_knorm*b_knorm) - (2*temp_k);
        double norm_l = (b_lnorm*b_lnorm) - (2*temp_l);	
        return make_pair(norm_k,norm_l);
    }
    else{
        MatrixXd e_l(p,n);
        MatrixXd e_k(p,n);
        for(int p_iter=0;p_iter<p;p_iter++)
        {
            for(int n_iter=0;n_iter<n;n_iter++){
                e_l(p_iter,n_iter) = g.get_geno(p_iter,n_iter,var_normalize) - b_l(p_iter,n_iter);
                e_k(p_iter,n_iter) = g.get_geno(p_iter,n_iter,var_normalize) - b_k(p_iter,n_iter);
            }
        }

        double ek_norm = e_k.norm();
        double el_norm = e_l.norm();
        return make_pair(ek_norm,el_norm);
    }
}

MatrixXd run_EM_not_missing(MatrixXd &c_orig){
	if(debug){
		print_time ();
		cout << "Enter: run_EM_not_missing" << endl;
	}
	MatrixXd c_temp(k,p);
	MatrixXd c_new(p,k);
	c_temp = ( (c_orig.transpose()*c_orig).inverse() ) * (c_orig.transpose());
	if(debug)
		print_timenl ();
	MatrixXd x_fn(k,n);
	multiply_y_post(c_temp,k,x_fn,true);
	if(debug)
		print_timenl ();
	MatrixXd x_temp(n,k);
	x_temp = (x_fn.transpose()) * ((x_fn*(x_fn.transpose())).inverse());
	multiply_y_pre(x_temp,k,c_new,true);
	if(debug){
		print_time ();
		cout << "Exiting: run_EM_not_missing" << endl;
	}
	return c_new;
}

MatrixXd run_EM_missing(MatrixXd &c_orig){
	MatrixXd c_new(p,k);

	MatrixXd mu(k,n);
	
	// E step
	MatrixXd c_temp(k,k);
	c_temp = (c_orig.transpose()*c_orig)  ;

	MatrixXd T(k,n);
	MatrixXd c_fn;
	c_fn = c_orig.transpose();
	multiply_y_post(c_fn,k,T,false);

	MatrixXd M_temp(k,1);
	for(int k_iter=0;k_iter<k;k_iter++){
		double sum=0.0;
		for(int p_iter=0;p_iter<p;p_iter++){
			sum+= c_orig(p_iter,k_iter)*g.get_col_mean(p_iter);
		}
		M_temp(k_iter,0) = sum;
	}

	for(int j=0;j<n;j++){
		MatrixXd D(k,k),M_to_remove(k,1);
		D = MatrixXd::Zero(k,k);
		M_to_remove = MatrixXd::Zero(k,1);
		for(int i=0;i<g.not_O_j[j].size();i++){
			cout<<"entered E step not "<<endl;
			D = D + (c_orig.row(i).transpose() * c_orig.row(i));
			M_to_remove = M_to_remove + (c_orig.row(i).transpose()*g.get_col_mean(i));
		}
		mu.col(j) = (c_temp-D).inverse() * ( T.col(j) - M_temp + M_to_remove);
	}

	// M step

	MatrixXd mu_temp(k,k);
	mu_temp = mu * mu.transpose();
	MatrixXd T1(p,k);
	MatrixXd mu_fn;
	mu_fn = mu.transpose();
	multiply_y_pre(mu_fn,k,T1,false);
	MatrixXd mu_sum(k,1);
	mu_sum = MatrixXd::Zero(k,1);
	for(int j=0;j<n;j++)
		mu_sum += mu.col(j);
	
	for(int i=0;i<p;i++){
		MatrixXd D(k,k),mu_to_remove(k,1);
		D = MatrixXd::Zero(k,k);
		mu_to_remove = MatrixXd::Zero(k,1);
		for(int j=0;j<g.not_O_i[i].size();j++){
			D = D + (mu.col(j) * mu.col(j).transpose());
			mu_to_remove = mu_to_remove + (mu.col(j));
		}
		c_new.row(i) = (((mu_temp-D).inverse()) * (T1.row(i).transpose() -  ( g.get_col_mean(i) * (mu_sum-mu_to_remove)))).transpose();
		double mean;
		mean = g.get_col_sum(i);
		mean = mean -  (c_orig.row(i)*(mu_sum-mu_to_remove))(0,0);
		mean = mean * 1.0 / (n-g.not_O_i[i].size());
		g.update_col_mean(i,mean);
	}

	return c_new;
}

MatrixXd run_EM(MatrixXd &c_orig){
	
	if(missing)
		return run_EM_missing(c_orig);
	else
		return run_EM_not_missing(c_orig);
}

void print_vals(){

	HouseholderQR<MatrixXd> qr(c);
	MatrixXd Q;
	Q = qr.householderQ() * MatrixXd::Identity(p,k);
	MatrixXd q_t(k,p);
	q_t = Q.transpose();
	MatrixXd b(k,n);
	multiply_y_post(q_t,k,b,true);
	JacobiSVD<MatrixXd> b_svd(b, ComputeThinU | ComputeThinV);
	MatrixXd u_l; 
	u_l = b_svd.matrixU();
	MatrixXd v_l;
	v_l = b_svd.matrixV();
	MatrixXd u_k;
	MatrixXd v_k,d_k;
	u_k = u_l.leftCols(k_orig);
	v_k = v_l.leftCols(k_orig);

	ofstream evec_file;
	evec_file.open((string(command_line_opts.OUTPUT_PATH)+string("evecs.txt")).c_str());
	evec_file<< std::setprecision(15) << Q*u_k << endl;
	evec_file.close();
	ofstream eval_file;
	eval_file.open((string(command_line_opts.OUTPUT_PATH)+string("evals.txt")).c_str());
	for(int kk =0 ; kk < k_orig ; kk++)
		eval_file << std::setprecision(15)<< (b_svd.singularValues())(kk)<<endl;
	eval_file.close();

	d_k = MatrixXd::Zero(k_orig,k_orig);
	for(int kk =0 ; kk < k_orig ; kk++)
		d_k(kk,kk)  =(b_svd.singularValues())(kk);

	MatrixXd x_k;
	x_k = d_k * (v_k.transpose());
	if(debug){
		ofstream c_file;
		c_file.open((string(command_line_opts.OUTPUT_PATH)+string("cvals.txt")).c_str());
		c_file<<c<<endl;
		c_file.close();
		ofstream x_file;
		x_file.open((string(command_line_opts.OUTPUT_PATH) + string("xvals.txt")).c_str());
		x_file<<x_k<<endl;
		x_file.close();
	}
}

int main(int argc, char const *argv[]){

	clock_t io_begin = clock();
    clock_gettime (CLOCK_REALTIME, &t0);

	pair<double,double> prev_error = make_pair(0.0,0.0);
	double prevnll=0.0;

	parse_args(argc,argv);

	memory_efficient = command_line_opts.memory_efficient;
    fast_mode = command_line_opts.fast_mode;
	missing = command_line_opts.missing;

	
	if(fast_mode){
		if(memory_efficient)
			g.read_genotype_eff(command_line_opts.GENOTYPE_FILE_PATH,missing);	
		else
			g.read_genotype_mailman(command_line_opts.GENOTYPE_FILE_PATH,missing);
	}
	else
		g.read_genotype_naive(command_line_opts.GENOTYPE_FILE_PATH,missing);	

    MAX_ITER =  command_line_opts.max_iterations ; 
	k_orig = command_line_opts.num_of_evec ;
	debug = command_line_opts.debugmode ;
	check_accuracy = command_line_opts.getaccuracy;
	var_normalize = command_line_opts.var_normalize;
	accelerated_em = command_line_opts.accelerated_em;
	k = k_orig + command_line_opts.l;
	p = g.Nsnp;
	n = g.Nindv;
	convergence_limit = command_line_opts.convergence_limit;
	srand((unsigned int) time(0));
	c.resize(p,k);
	x.resize(k,n);
	v.resize(p,k);
	means.resize(p,1);

	clock_t io_end = clock();

	//TODO: Initialization of c

	c = MatrixXd::Random(p,k);

	
	for(int i=0;i<p;i++)
		means(i,0) = g.get_col_mean(i);
	
	ofstream c_file;
	if(debug){
		c_file.open((string(command_line_opts.OUTPUT_PATH)+string("cvals_orig.txt")).c_str());
		c_file<<c<<endl;
		c_file.close();
		printf("Read Matrix\n");
	}

	cout<<"Running on Dataset of "<<g.Nsnp<<" SNPs and "<<g.Nindv<<" Individuals"<<endl;

	
	clock_t it_begin = clock();
	for(int i=0;i<MAX_ITER;i++){

		MatrixXd c1,c2,cint,r,v;
		double a,nll;
		if(debug){
			print_time (); 
			cout << "*********** Begin epoch " << i << "***********" << endl;
		}
		if(accelerated_em!=0){
			if(debug){
				print_time();
				cout << "Before EM" << endl;
			}
			c1 = run_EM(c);
			c2 = run_EM(c1);
			if(debug){
				print_time(); 
				cout << "After EM but before acceleration" << endl;
			}
			r = c1-c;
			v = (c2-c1) - r;
			a = -1.0 * r.norm() / (v.norm()) ;
			if(accelerated_em==1){
				if(a>-1){
					a=-1;
					cint=c2;
				}
				else {
					cint = c - 2*a*r + a*a*v;
					nll = get_error_norm(cint).second;
					if(i>0){
						while(nll>prevnll && a<-1){
							a = 0.5 * (a-1);
							cint = c - 2*a*r +(a*a*v);
							nll = get_error_norm(cint).second;
						}
					}
				}
				c = cint;
			}
			else if(accelerated_em==2){
				cint = c - 2*a*r + a*a*v;
				c = cint;
				// c = run_EM(cint);				
			}
		}
		else{
			c = run_EM(c);
		}
		
		if ( accelerated_em == 1 || check_accuracy ) {
			pair<double,double> e = get_error_norm(c);
			prevnll = e.second;
			if(check_accuracy) 
				cout<<"Iteration "<<i+1<<"  "<<e.first<<"  "<<e.second<<endl;
			prev_error = e;
		}
		if(debug){
			print_time (); 
			cout << "*********** End epoch " << i << "***********" << endl;
		}

	}
	clock_t it_end = clock();

    print_vals();
		
	clock_t total_end = clock();
	double io_time = double(io_end - io_begin) / CLOCKS_PER_SEC;
	double avg_it_time = double(it_end - it_begin) / (MAX_ITER * 1.0 * CLOCKS_PER_SEC);
	double total_time = double(total_end - total_begin) / CLOCKS_PER_SEC;
	cout<<"IO Time:  "<< io_time << "\nAVG Iteration Time:  "<<avg_it_time<<"\nTotal runtime:   "<<total_time<<endl;
	return 0;
}
