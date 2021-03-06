/*
 *  sherpa.cpp
 *
 *  Copyright 2007-2017 Jeffrey Newman
 *
 *  Larch is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  Larch is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with Larch.  If not, see <http://www.gnu.org/licenses/>.
 *  
 */


#define NOMINMAX
#include <limits>
#include "sherpa.h"
#include "sherpa_freedom.h"
#include <iostream>
#include <iomanip>

#define PERTURBATION_SIZE (1e-5)

using namespace etk;
using namespace std;


double sherpa::objective() 
{ 
	OOPS("error(sherpa): default objective");
	return 0; 
}
const etk::memarray& sherpa::gradient(const bool& force_recalculate)
{ 
	negative_finite_diff_gradient_(GLastTurn);
	return GLastTurn;
}

void sherpa::calculate_hessian()   
{
	finite_diff_hessian(Hess);
}


void sherpa::negative_finite_diff_gradient_(memarray& fGrad)
{
	unsigned i;	
	double jiggle;
	if (fGrad.size() < dF()) OOPS("error(sherpa): not enough finite diff array space");
	
	for (i=0;i<dF();i++) {
		jiggle = FCurrent[i] * PERTURBATION_SIZE;
		if (!jiggle) jiggle = PERTURBATION_SIZE;
		
		FCurrent[i] += jiggle;
		freshen();
		fGrad[i] = objective();
		FCurrent[i] -= jiggle;
		
		FCurrent[i] -= jiggle;
		freshen();
		fGrad[i] -= objective();
		FCurrent[i] += jiggle;
		freshen();
		
		fGrad[i] /= -2*jiggle;
	}
}

void sherpa::finite_diff_gradient_(memarray& fGrad)
{
	unsigned i;	
	double jiggle;
	if (fGrad.size() < dF()) OOPS("error(sherpa): not enough finite diff array space");
	
	for (i=0;i<dF();i++) {
		jiggle = FCurrent[i] * PERTURBATION_SIZE;
		if (!jiggle) jiggle = PERTURBATION_SIZE;
		
		FCurrent[i] += jiggle;
		freshen();
		fGrad[i] = objective();
		FCurrent[i] -= jiggle;
		
		FCurrent[i] -= jiggle;
		freshen();
		fGrad[i] -= objective();
		FCurrent[i] += jiggle;
		freshen();
		
		fGrad[i] /= 2*jiggle;
	}
}


void sherpa::finite_diff_hessian (triangle& fHESS) 
{
	// Formulation from Givens & Hoeting, "Computational Statistics"
	
	memarray temp_fHess (dF(),dF());
	unsigned i,j;
	double jiggle;
		
	for (i=0;i<dF();i++) {
		jiggle = ReadFCurrent()[i] * PERTURBATION_SIZE;
		if (!jiggle) jiggle = PERTURBATION_SIZE;
		
		FCurrent[i] += jiggle;
		freshen();
		objective();
		gradient(); 
		cblas_dcopy(dF(), GCurrent.ptr(),1, temp_fHess.ptr()+i,dF());    
		FCurrent[i] -= jiggle;
		
		FCurrent[i] -= jiggle;
		freshen();
		objective();
		gradient();     
		cblas_daxpy(dF(), -1, GCurrent.ptr(),1, temp_fHess.ptr()+i,dF());
		FCurrent[i] += jiggle;
		freshen();
		
		if (jiggle) cblas_dscal(dF(), 1/(2*jiggle), temp_fHess.ptr()+i,dF());						
		//MONITOR(msg) << "xHESSIAN\n" << temp_fHess.printrows(0,dF()) ;
	}
	
	fHESS.initialize();
	for (i=0;i<dF();i++) for (j=0;j<dF();j++) {
		fHESS(i,j) += temp_fHess(i,j);
		if (i==j) fHESS(i,j) += temp_fHess(i,j);
		//MONITOR(msg) << "fHESSIAN\n" << fHESS.printSquare() ;
	}
	fHESS.scale(0.5);
}

// DIAGNOSTICS //

double sherpa::gradient_diagnostic (bool shout)
{
	unsigned i;
	INFO(msg) << "Conducting Gradient Diagnostic..." ;
	gradient(); 
	memarray FiniteGrad (dF());
	negative_finite_diff_gradient_(FiniteGrad);

	std::ostringstream buff;
	buff << "Parameter         \tParamValue        \tAnalyticGrad      FiniteDiffGrad    \n" ;
	for ( i=0; i<dF(); i++) {
		buff.width(18);
		buff << FNames[i] << "\t";
		buff.width(18);
		buff << ReadFCurrent()[i] << "\t";
		buff.width(18);
		buff << GCurrent[i] << "";
		buff.width(18);
		buff << FiniteGrad[i] << "\n";
	}
	
	FiniteGrad -= GCurrent;
	double max_diff = 0;
	
	for (i=0; i<dF(); i++) {
		if ((fabs(FiniteGrad[i]) > max_diff) && FiniteGrad[i] && GCurrent[i])
			max_diff = fabs(FiniteGrad[i]);
	}
	
	double max_percent_diff = 0;
	bool found_nan =false;
	for (i=0; i<dF(); i++) {
		if ((fabs(FiniteGrad[i]/GCurrent[i]) > max_percent_diff) && FiniteGrad[i]!=0 && GCurrent[i]!=0)
			max_percent_diff = fabs(FiniteGrad[i]/GCurrent[i]);
		if (isNan(GCurrent[i])) found_nan = true;
	}
	
	buff << "Maximum Absolute Deviation:   " << max_diff << "\n";
	buff << "Maximum Percentage Deviation: " << max_percent_diff*100 << "%";
	
	if (shout) {
		SHOUT(msg)<< buff.str() ;
	} else {
		INFO(msg)<< buff.str() ;
	}
	if (max_percent_diff > 0.09 || found_nan) {
		OOPS("error: gradient diagnostic disagreement\n",buff.str());
		//msg << "Press enter to continue...\n";
		//std::cin.get();
	}
	
	return max_diff;
}


double sherpa::hessian_diagnostic () 
{
	unsigned i,j;
	INFO(msg)<< "Conducting Hessian Diagnostic..." ;
	calculate_hessian();
	triangle FiniteHess (dF());
	finite_diff_hessian(FiniteHess);
	
	ostringstream buff;
	buff << "Parameter           \tParameter           \tAnalyticHess        \tFiniteDiffHess(+/-)      \n" ;
	for ( i=0; i<dF(); i++) {
	}
	
	FiniteHess -= Hess;
	double max_diff = 0;
	
	for (i=0; i<dF(); i++) for (j=i; j<dF(); j++) {
		if ((fabs(FiniteHess(i,j)) > max_diff) && FiniteHess(i,j) && Hess(i,j))
			max_diff = fabs(FiniteHess(i,j));
	}
	
	double max_percent_diff = 0;
	bool found_nan =false;
	for (i=0; i<dF(); i++)  for (j=i; j<dF(); j++) {
		if ((fabs(FiniteHess(i,j)/Hess(i,j)) > max_percent_diff) && FiniteHess(i,j)!=0 && Hess(i,j)!=0) {
			max_percent_diff = fabs(FiniteHess(i,j)/Hess(i,j));
			if ((fabs(FiniteHess(i,j)/Hess(i,j)) > 0.001)) {
				buff.width(20);
				buff << FNames[i] << "\t";
				buff.width(20);
				buff << FNames[j] << "\t";
				buff.width(20);
				buff << Hess(i,j) << "\t";
				buff.width(20);
				buff << FiniteHess(i,j) << "\n";
			}
		}
		if (isNan(Hess(i,j))) found_nan = true;
	}
	
	buff << "Maximum Absolute Deviation:   " << max_diff << "\n";
	buff << "Maximum Percentage Deviation: " << max_percent_diff*100 << "%";
	
	INFO(msg)<< buff.str() ;
	if (max_percent_diff > 0.09 || found_nan) {
		OOPS("error: hessian diagnostic disagreement\n",buff.str());
		//msg << "Press enter to continue...\n";
		//std::cin.get();
	}
	
	return max_diff;
}


double sherpa::_check_for_improvement()
{
	double improvement = 0;
	if (isNan(ZCurrent) || isNan(ZBest)) {
		improvement = 0;
	} else {
		improvement = ZCurrent - ZBest;
	}
	if (ZCurrent > ZBest) {
		ZBest = ZCurrent;
		FBest = ReadFCurrent();
	}
	return improvement;
}

void sherpa::_reset_to_best_known_value()
{
	// ZCurrent = ZBest;
	FCurrent = FBest;
	freshen();
	ZCurrent = objective();
}


// ----------------------------------------------------------------------------
//	_line_search
//
//		Returns the amount of improvement if finite (can be negative)
//      or zero for errors
// ----------------------------------------------------------------------------
double sherpa::_line_search_evaluation(double& step)
{
	if (PyErr_CheckSignals()) PYTHON_INTERRUPT;

	double* FLastTurn_ptr = FLastTurn.ptr()+1;
	double* FDirection_ptr = FDirection.ptr()+1;
	double* FCurrent_ptr = FCurrent.ptr()+1;


	FCurrent.projection(FLastTurn,FDirection,step);
	
	for (unsigned i=0; i<dF(); i++) {
		if (ReadFCurrent()[i] > FMax[i]) {
			INFO(msg)<< "Line search wants parameter "<<i
			<<" to be "<<ReadFCurrent()[i]<<", exceeding the max bound "
			<<FMax[i]<<"; I am reducing it";
			//FCurrent[i] = FMax[i] + log(1.+FCurrent[i]-FMax[i]);
			FCurrent[i]= FMax[i];
		}
		if (ReadFCurrent()[i] < FMin[i]) {
			INFO(msg)<< "Line search wants parameter "<<i
			<<" to be "<<FCurrent[i]<<", undermining the min bound "
			<<FMin[i]<<"; I am increasing it";
			//FCurrent[i] = FMin[i] - log(1.-FCurrent[i]+FMin[i]);
			FCurrent[i] = FMin[i];
		}
	}
//	freshen();
	ZCurrent = objective();
	return _check_for_improvement();
}

int sherpa::_line_search (sherpa_pack& method)
{

	MONITOR(msg) << "Turn(["<< ReadFLastTurnAsString() <<"])";
	
	int status = 0;	
	double Step = method.get_step();
	
	double improvement = _line_search_evaluation(Step);
	double total_improvement = improvement;
	
	if (isNan(ZCurrent)) status = LINE_SEARCH_ERROR_NAN;
	if (improvement > 0) {
		status = LINE_SEARCH_SUCCESS_BIG; 
		MONITOR(msg)<< "     first step line search improvement="<<improvement ;
	}
	
	// When First Step was an improvement, potentially try some more
	if ((status==LINE_SEARCH_SUCCESS_BIG) && (Step < method.Max_Step)) {
		do {
			Step *= method.Step_Extend_Factor;
			MONITOR(msg)<< "seeking further line search improvement with stepsize="<<Step ;
			improvement = _line_search_evaluation(Step);
			if (improvement>0) total_improvement += improvement;
			MONITOR(msg)<< "        further line search improvement="<<improvement ;
		} while ( (improvement>0) && (Step < method.Max_Step) );
		if (ZBest!=ZCurrent) {
			// we extended too far, need to back up a step
			Step /= method.Step_Extend_Factor;
			_line_search_evaluation(Step);	
		}
	}
	
	if (status==LINE_SEARCH_SUCCESS_BIG) {
		INFO(msg)<< "  using "<<algorithm_name(method.Algorithm)
		         <<", line search found improvement to "
				 << ZCurrent <<" (+"<<total_improvement<<") using stepsize="<<Step ;
	}
	
	// When First Step was NOT an improvement
	while (status==LINE_SEARCH_NO_IMPROVEMENT || status==LINE_SEARCH_ERROR_NAN) {
		BUGGER(msg)<< "line search found degradation to "<< ZCurrent <<" (-"<<-improvement<<") using stepsize="<<Step ;
		Step *= method.Step_Retract_Factor;
		improvement = _line_search_evaluation(Step);
		if (isNan(ZCurrent)) status = LINE_SEARCH_ERROR_NAN;
		if (Step < method.Min_Step) status = LINE_SEARCH_FAIL;
		if (improvement > 0) {
			INFO(msg)<< "  using "<<algorithm_name(method.Algorithm)
			         <<", line search found improvement to "
			         << ZCurrent <<" (+"<<improvement<<") using stepsize="<<Step ;
			status = LINE_SEARCH_SUCCESS_SMALL;
		}
	}
	
	if (isNan(ZCurrent)) status = LINE_SEARCH_ERROR_NAN;
		
	if (status > 0) {
		method.tell_step(Step);
	} else {
		_reset_to_best_known_value();
	}
	return status;
	
}

// TODO: Identify when maximization has every parameter but one alternating sign direction over many iterations

int sherpa::_bhhh_update (etk::symmetric_matrix* use_bhhh)
{

	if (!use_bhhh) {
		return -2;
	}

	if (use_bhhh->all_zero()) {
		return -1;
	}

//	BUGGER(msg) << "bhhh=\n" << Bhhh.printSquare() ;
	invHessTemp = *use_bhhh;
//	BUGGER(msg) << "invHessTemp1=\n" << invHessTemp.printSquare() ;
	
	if (any_holdfast()) {

		symmetric_matrix temp_free_hess (invHessTemp.size1()-count_holdfast());
		
		// invert hessian
		hessfull_to_hessfree(&invHessTemp, &temp_free_hess) ;
		temp_free_hess.inv();
		hessfree_to_hessfull(&invHessTemp, &temp_free_hess) ;

	} else {
		
//		std::cerr << "#  #   GCurrent   .ptr()="<<&GCurrent<<"\n";
//		std::cerr << "#  #   invHessTemp.ptr()="<<&invHessTemp<<"\n";
//		std::cerr << "#  #   invHessTemp.size()="<<invHessTemp.size1()<<"\n";
//		std::cerr << "#  #   invHessTemp.size()="<<invHessTemp.size2()<<"\n";
//		std::cerr << "#  #   invHessTemp.size()="<<invHessTemp.size3()<<"\n";
		
		invHessTemp.inv(&msg);
	}
		
//	BUGGER(msg) << "invHessTemp=\n" << invHessTemp.printSquare() ;
	return 0;
}

int sherpa::_bfgs_update () 
{
	double Denom, DenomB;
	
	invHessTemp = invHess;
	
	Denom = 1/(FMotion * GMotion);
	
	// Breakout when the difference is too small
	if (isNan(Denom)) return -1;
	if (fabs(Denom) > 1e30) return -1;
	
	ndarray HdG (dF());
	#ifdef SYMMETRIC_PACKED
	cblas_dspmv(CblasRowMajor,CblasUpper, dF(), 1,
				*invHessTemp, *GMotion,1, 0, *HdG,1);
	cblas_dspr2(CblasRowMajor, CblasUpper, 
				dF(), -Denom, *HdG,1, *FMotion,1, *invHessTemp);
	#else
	cblas_dsymv(CblasRowMajor,CblasUpper, dF(), 1,
				*invHessTemp, invHessTemp.size1(), *GMotion,1, 0, *HdG,1);
	cblas_dsyr2(CblasRowMajor, CblasUpper,
				dF(), -Denom, *HdG,1, *FMotion,1, *invHessTemp, invHessTemp.size1());
	#endif
	
	DenomB = (GMotion * HdG);
	DenomB *= Denom;
	DenomB += 1;
	DenomB *= Denom;
	
	// Breakout when the difference is too small
	if (isNan(DenomB)) return -1;
	
	#ifdef SYMMETRIC_PACKED
	cblas_dspr(CblasRowMajor, CblasUpper, 
			   dF(), DenomB, *FMotion,1, *invHessTemp);
	#else
	cblas_dsyr(CblasRowMajor, CblasUpper,
			   dF(), DenomB, *FMotion,1, *invHessTemp, invHessTemp.size1());
	#endif
	return 0;
}


int sherpa::_dfp_update () 
{
	double Denom, DenomB;
	puddle HdG (dF());	
	
	invHessTemp = invHess;
	
	Denom = 1/(FMotion * GMotion);
	if (isNan(Denom)) return -1;
	if (fabs(Denom) > 1e30) return -1;
	#ifdef SYMMETRIC_PACKED
	cblas_dspmv(CblasRowMajor,CblasUpper, dF(), 1,
				*invHessTemp, *GMotion,1, 0, *HdG,1);
	#else
	cblas_dsymv(CblasRowMajor,CblasUpper, dF(), 1,
				*invHessTemp, invHessTemp.size1(), *GMotion,1, 0, *HdG,1);
	#endif
 	DenomB = -1/cblas_ddot(dF(), *GMotion,1, *HdG,1);
	#ifdef SYMMETRIC_PACKED
	cblas_dspr(CblasRowMajor, CblasUpper, dF(), DenomB, *HdG,1, *invHessTemp);
	cblas_dspr(CblasRowMajor, CblasUpper, dF(), Denom, *FMotion,1, *invHessTemp);
	#else
	cblas_dsyr(CblasRowMajor, CblasUpper, dF(), DenomB, *HdG,1, *invHessTemp, invHessTemp.size1());
	cblas_dsyr(CblasRowMajor, CblasUpper, dF(), Denom, *FMotion,1, *invHessTemp, invHessTemp.size1());
	#endif
	return 0;
}


int sherpa::_dfpj_update ()
{
	memarray HdG (dF());
	invHessTemp = invHess;
	
    double Denom, Numerat;
    Denom = 1/(FMotion * GMotion);
	if (isNan(Denom)) return -1;
	
	// Recycle prevGrad memory space 
	#ifdef SYMMETRIC_PACKED
	cblas_dspmv(CblasRowMajor,CblasUpper, dF(),
				-1,
				*invHessTemp,
				*GMotion,1,
				0,
				*HdG,1);
	#else
	cblas_dsymv(CblasRowMajor,CblasUpper, dF(),
				-1,
				*invHessTemp,invHessTemp.size1(),
				*GMotion,1,
				0,
				*HdG,1);
	#endif
	HdG += FMotion;
	
	#ifdef SYMMETRIC_PACKED
	cblas_dspr2(CblasRowMajor, CblasUpper,
				dF(), Denom, 
				*HdG,1, 
				*FMotion,1, 
				*invHessTemp);
	#else
	cblas_dsyr2(CblasRowMajor, CblasUpper,
				dF(), Denom, 
				*HdG,1, 
				*FMotion,1, 
				*invHessTemp,invHessTemp.size1());
	#endif
	
	Numerat = (HdG * GMotion) * Denom * Denom * -1;
	if (isInf(Numerat)) return -1;
	if (isNan(Numerat)) return -1;
	#ifdef SYMMETRIC_PACKED
	cblas_dspr(CblasRowMajor, CblasUpper, dF(), Numerat, *FMotion,1, *invHessTemp);
	#else
	cblas_dsyr(CblasRowMajor, CblasUpper, dF(), Numerat, *FMotion,1, *invHessTemp, invHessTemp.size1());
	#endif
	return 0;
}



int sherpa::_find_ascent_direction (char& Method) 
{
	MONITOR(msg)<< "Seeking ascent direction" ;
	int status (0);
	switch (Method) {
			
			// Analytic or finite difference Hessian
		case 'A':
		case 'a':
			MONITOR(msg)<< "using hessian to seek ascent direction" ;
			calculate_hessian();
			invHessTemp = Hess;
			invHessTemp.inv();
			break; 
			
			// BFGS (Broyden-Fletcher-Goldfarb-Shanno) Algorithm	
		case 'B':	
		case 'b':	
			MONITOR(msg)<< "using BFGS to seek ascent direction" ;
			status = _bfgs_update();
			if (status<0) WARN(msg)<< "improvement is too small for BFGS, new "
				"inverse hessian estimate not calculated";
			break;
			
			// DFP (Davidson-Fletcher-Powell) Algorithm
		case 'D':
		case 'd':	
			MONITOR(msg)<< "using DFP to seek ascent direction" ;
			status = _dfp_update();
			if (status<0) WARN(msg)<< "improvement is too small for DFP, new "
				"inverse hessian estimate not calculated" ;
			break;
			
			// Jeff's Undocumented DFP Variant
		case 'J':
		case 'j':	
			MONITOR(msg)<< "using DFP(J) to seek ascent direction" ;
			status = _dfpj_update();
			if (status<0) WARN(msg)<< "improvement is too small for DFP(J), new "
				"inverse hessian estimate not calculated" ;
			break;
			
			// Steepest Ascent
		case 'S':
		case 's':	
			MONITOR(msg)<< "using Steepest Ascent direction" ;
			invHessTemp.initialize_identity();
			break;
			
			// BHHH (Berndt-Hall-Hall-Hausman) Algorithm
		case 'G':
		case 'g':	
		default:
			MONITOR(msg)<< "using BHHH to seek ascent direction" ;
			status = _bhhh_update(&Bhhh);
			if (status<0) {
				WARN(msg)<< "BHHH is missing, so using BFGS to seek ascent direction" ;
				status = _bfgs_update();
				if (status<0) WARN(msg)<< "improvement is too small for BFGS, new "
					"inverse hessian estimate not calculated";
			}
			break;
	}

	FDirection.initialize(0.0);
	
	#ifdef SYMMETRIC_PACKED
	cblas_dspmv(CblasRowMajor,CblasUpper,dF(), -1, 
				*invHessTemp, 
				*GCurrent,1, 
				0,*FDirection,1);
	#else
	cblas_dsymv(CblasRowMajor,CblasUpper,dF(), -1,
				*invHessTemp, invHessTemp.size1(),
				*GCurrent,1, 
				0,*FDirection,1);
	#endif
	
//	MONITOR(msg) << "FDirection=\n" << FDirection.print()  ;
	
	return status;
}

void sherpa::_basecamp()
{
	GPrevTurn = GLastTurn;
	FPrevTurn = FLastTurn;
	
	GLastTurn = GCurrent;
	FLastTurn = ReadFCurrent();
	
	FMotion = FLastTurn;
	FMotion -= FPrevTurn;
	
	GMotion = GLastTurn;
	GMotion -= GPrevTurn;
}

void sherpa::full_evaluation(int with_derivs)
{
	ZCurrent = objective();
	if (with_derivs >= 1) gradient();
	if (with_derivs >= 2) calculate_hessian();
}

void sherpa::_initial_evaluation()
{
	BUGGER(msg)<< "Initial evaluation" ;
	full_evaluation();
	ZBest = ZCurrent;
	FBest = ReadFCurrent();
	_basecamp();
}

#define return_outcome(x) outcome.best_obj_value=ZCurrent; outcome.result=(x); return outcome

sherpa_result sherpa::_maximize_pack(sherpa_pack& Norgay, unsigned& iteration_number)
{
	BUGGER(msg)<< "Maximizing based on sherpa_pack" << Norgay.print_pack() ;
	
	sherpa_result outcome;
	
	// Initial Evaluation
	if (!iteration_number) _initial_evaluation();
	
	double tolerance; 
	int status;
	string ret;
	bool keep_going = true;
	bool any_improvement = false;
	unsigned local_iteration (0);
	outcome.starting_obj_value = -INF;
	double previous_obj_value (-INF);
	
	etk::dblvec Recorded_Objective_Values;

//  ## initialization of invHess is only done once, we use the old estimate when switching packs...
//	if (Norgay.Algorithm=='b' || Norgay.Algorithm=='B' ||
//		Norgay.Algorithm=='d' || Norgay.Algorithm=='D' ||
//		Norgay.Algorithm=='j' || Norgay.Algorithm=='J' ) {
//		invHess.initialize_identity();
//	}
	
	while (keep_going) {
		std::ostringstream message;
		iteration_number++;
		if (iteration_number > max_iterations) {
			message << "Exceeded Maximum Number of Iterations ("<<max_iterations<<")";
			WARN(msg) << message.str();
			outcome.explain_stop = message.str();
			return_outcome( SHERPA_MAXITER );
		}
		local_iteration++;
		MONITOR(msg) << "=================================================" ;
		MONITOR(msg)<< "ITERATION NUMBER "<<iteration_number<< " BEGINS" ;
		MONITOR(msg) << "=================================================" ;
		objective();
		if (outcome.starting_obj_value==-INF) outcome.starting_obj_value = ZCurrent;
		if (previous_obj_value==-INF) previous_obj_value = ZCurrent;
		gradient();
		
		int direction_status = _find_ascent_direction(Norgay.Algorithm);
		tolerance = -(FDirection*GCurrent);
		
		if (flag_log_turns) {
			FATAL(msg) << "Log Likelihood = "<<ZCurrent << "\t"<<Norgay.AlgorithmName()<<" Convergence Tolerance = "<<tolerance<<" @\n" << printStatus(status_FNames|status_FCurrent|status_GCurrent|status_FDirection|status_FDirectionLarge, tolerance) ;
		} else {
			MONITOR(msg) << "\n" << printStatus(status_FNames|status_FCurrent) ;
		}
		
		if ((tolerance==0.0)&&(direction_status<0)) {
			return_outcome( SHERPA_FAIL );
		}
		if (isNan(tolerance)) {
			return_outcome( SHERPA_NAN_TOL );
		}
		
		keep_going = Norgay.tell_turn(ZCurrent,tolerance,outcome.explain_stop, local_iteration);
		if (!keep_going) {
			MONITOR(msg) << "+++++++++++++++++++++++++++++++++++++++++++++++++" ;
			WARN(msg)<< "success: "<< outcome.explain_stop ;
			MONITOR(msg) << "+++++++++++++++++++++++++++++++++++++++++++++++++" ;
			MONITOR(msg) << "\n" << printStatus(status_FNames|status_FCurrent|status_GCurrent|status_FDirection) ;
			return_outcome( SHERPA_SUCCESS );
		}
		
		INFO(msg)<< "Iteration "<< iteration_number <<": Convergence Measure = "<<tolerance;
		MONITOR(msg) << "\n" << printStatus(status_FNames|status_FCurrent|status_GCurrent|status_FDirection) ;
		
		if (flag_gradient_diagnostic) {
			if (flag_gradient_diagnostic>10) {
				SHOUT(msg)<< "Running gradient diagnostic";
			} else {
				INFO(msg)<< "Running gradient diagnostic";
			}
			gradient_diagnostic(flag_gradient_diagnostic>10);
			flag_gradient_diagnostic--;
		}
		if (flag_hessian_diagnostic) {
			hessian_diagnostic();
			flag_hessian_diagnostic--;
		}
		
		status = _line_search(Norgay);
		
		Recorded_Objective_Values.push_back(ZCurrent);
		
		// Report the results of this direction
		if (status>0) {
			MONITOR(msg)<< "Improved "<< ZCurrent-previous_obj_value <<" on this iteration using " << algorithm_name(Norgay.Algorithm) ;
			any_improvement = true;
		}
		if (status==SHERPA_IMP_STUCK) {
			if (any_improvement) {
				message << "Improved "<< ZCurrent-outcome.starting_obj_value
				          <<" over "<<local_iteration<<" iterations"
						  <<" but got stuck by using "<< algorithm_name(Norgay.Algorithm) ;
				WARN(msg) << message.str();
				MONITOR(msg) << "Current Best LL="<<ZBest ;
				outcome.explain_stop = message.str();
				return_outcome( SHERPA_IMP_STUCK );
			} else {
				message << "Failed to Improve Using "<< algorithm_name(Norgay.Algorithm) ;
				WARN(msg) << message.str();
				outcome.explain_stop = message.str();
				return_outcome( SHERPA_FAIL );
			}
		}
		if (status==SHERPA_IMP_ERROR) {
			if (any_improvement) {
				message << "Improved "<< ZCurrent-outcome.starting_obj_value <<" over "
				          <<local_iteration<<" iterations but stuck on an error using "
						  << algorithm_name(Norgay.Algorithm) ;
				WARN(msg) << message.str();
				outcome.explain_stop = message.str();
				return_outcome( SHERPA_IMP_ERROR );
			} else {
				message << "Encountered an Error Using "<< algorithm_name(Norgay.Algorithm) ;
				ERRO(msg) << message.str();
				outcome.explain_stop = message.str();
				return_outcome( SHERPA_BAD );
			}
		}
			
		
		// If some improvement, accept the provisional hessian
		invHess = invHessTemp;
		// Set Turn
		_basecamp();
		previous_obj_value = ZCurrent;
				
		unsigned rememberance (5);
		
		// Check for Slowness
		if (local_iteration > Norgay.Honeymoon) {
			double average_improve = Recorded_Objective_Values.moving_average_improvement(rememberance);
			if (average_improve < Norgay.Patience) {
				message << "At iteration "<<iteration_number<<", "
						  << algorithm_name(Norgay.Algorithm) << " is improving too slowly (average improvement "
						  <<average_improve << " over " << Recorded_Objective_Values.moving_average_improvement_size(rememberance)
						  << " iterations)" ;
				WARN(msg) << message.str();
				outcome.explain_stop = message.str();
				return_outcome( SHERPA_SLOW );
			} else {
				MONITOR(msg) << algorithm_name(Norgay.Algorithm) << " is improving acceptably (average improvement "
				<<average_improve << " over " << Recorded_Objective_Values.moving_average_improvement_size(rememberance) << " iterations)" ;
			}
		} else {
			MONITOR(msg) << algorithm_name(Norgay.Algorithm) << " is still in the honeymoon period ("
			<<Norgay.Honeymoon - local_iteration << " iterations remaining)" ;
		}
	
		if (PyErr_CheckSignals()) PYTHON_INTERRUPT;
	}
	
	return_outcome( SHERPA_FATAL );
	
}

sherpa_pack* __find_unfailed_sherpa_pack(vector<sherpa_pack>& p)
{
	sherpa_pack* tentative = NULL;
	for (unsigned i=0; i<p.size(); i++) {
		if (!p[i].Fail) {
			if (tentative) {
				if (p[i].Slowness < tentative->Slowness) {
					tentative = &(p[i]);
				}
			} else {
				tentative = &(p[i]);
			}
		}
	}
	return tentative;
}

void __unfail_all_sherpa_packs(vector<sherpa_pack>& p, sherpa_pack* except=NULL)
{
	for (unsigned i=0; i<p.size(); i++) {
		if (&(p[i]) != except) p[i].Fail = false;
	}
}

string sherpa::maximize(unsigned& iteration_number, vector<sherpa_pack>* opts)
{
	BUGGER(msg)<< "beginning maximization" ;
	
	vector<sherpa_pack> packs;
	//(char algo, double thresh,    double ini,  double min, double max, double ext, double ret,  unsigned honey, double pat)
	packs.push_back( sherpa_pack('G', 0.000001, 1, 0,   1e-10, 4, 2, .5,    1, 0.001) );
//	packs.push_back( sherpa_pack('D', 0.000001) );
	packs.push_back( sherpa_pack('B', 0.000001) );
//	packs.push_back( sherpa_pack('J', 0.000001) );
	packs.push_back( sherpa_pack('S', 0.000001, 1.e-6, 100,   1e-10, 4, 2, .5,    1, 100.) );
	if (!opts) opts = &packs;
	
	sherpa_result status;
	double net_improvement = 0;
	
	__unfail_all_sherpa_packs(*opts);
	sherpa_pack* current_pack = __find_unfailed_sherpa_pack(*opts);
	
	if (!current_pack) OOPS("error in maximization -- no optimization pack found");
	
	_initialize();
	
	invHess.initialize_identity();
	
	while (current_pack) {
		status = _maximize_pack(*current_pack, iteration_number);
		net_improvement += (status.best_obj_value - status.starting_obj_value);
		if (status.result == SHERPA_SUCCESS) {
			INFO(msg)<< "successful maximization" ;
			return "success";
		}
		if (status.result >= -2) __unfail_all_sherpa_packs(*opts,current_pack);
		if (status.result < 0) current_pack->Fail = true;
		if (status.result == 0) current_pack->Slowness += 1;
		current_pack = __find_unfailed_sherpa_pack(*opts);
		if (current_pack) {
			INFO(msg)<< "trying " << algorithm_name(current_pack->Algorithm);
		}
	}
	WARN(msg)<< "unsuccessful maximization" ;
	
	if (net_improvement>0) {
		ostringstream message;
		message << "failure: improved log likelihood by "<<net_improvement<<" over initial value but got stuck";
		return message.str();
	}
	
	return "failure: did not find any improvement in log likelihood";
}

string sherpa::calculate_errors()
{
	calculate_hessian();
	TODO;
	return "success";
}


bool sherpa::parameter_exists (const string& freedom_name) const
{
	return FNames.has_key(freedom_name);

}


double sherpa::parameter_value (const string& freedom_name) const
{
	if (FNames.has_key(freedom_name)) {
		return FCurrent[ FNames[freedom_name] ];
	}
	return NAN;
}

#include "larch_modelparameter.h"

double sherpa::parameter_stderr(const string& freedom_name) const
{
	if (FNames.has_key(freedom_name)) {
		elm::ModelParameter mp ( const_cast<sherpa*> (this), FNames[freedom_name]);
		return mp._get_std_err();
	}
	return NAN;
}



void sherpa::_update_freedom_info_best()
{
	
	for (unsigned i=0; i<dF(); i++) {
		FCurrent[i] = ReadFBest()[i];
	}
}






void sherpa::reset_to_initial_value()
{
	FCurrent = FInitValues;
}
void sherpa::refresh_initial_value()
{
	FInitValues = FCurrent;
}



void sherpa::resize_allocated_memory()
{
	size_t number_of_parameters = FNames.size();

	FCurrent.resize(number_of_parameters);
	FHoldfast.resize_int8(number_of_parameters);
	FInitValues.resize(number_of_parameters);
	FNullValues.resize(number_of_parameters);
	_FCurrent_latest_objective.resize(number_of_parameters);
	if (number_of_parameters>0) {
		_FCurrent_latest_objective[0] = NAN;
	}
	FBest.resize(number_of_parameters);
	FLastTurn.resize(number_of_parameters);
	FPrevTurn.resize(number_of_parameters);
	FMotion.resize(number_of_parameters);
	FDirection.resize(number_of_parameters);
	FMax.resize(number_of_parameters);
	FMin.resize(number_of_parameters);
	GCurrent.resize(number_of_parameters);
	FatGCurrent.resize(number_of_parameters);
	GLastTurn.resize(number_of_parameters);
	GPrevTurn.resize(number_of_parameters);
	GMotion.resize(number_of_parameters);
	Bhhh.resize(number_of_parameters);
	Hess.resize(number_of_parameters);
	invHess.resize_nan(number_of_parameters);
	invHessTemp.resize_nan(number_of_parameters);
	robustCovariance.resize_nan(number_of_parameters);
	hessian_matrix.resize_nan(number_of_parameters);

}


void sherpa::allocate_memory()
{
	resize_allocated_memory();
	_initialize();


//	if (FCurrent.size1()!=dF()) {
//	
//		FCurrent.resize(dF());
//		_FCurrent_latest_objective.resize(dF());
//		if (dF()>0) {
//			_FCurrent_latest_objective[0] = NAN;
//		}
//		FBest.resize(dF());
//		FLastTurn.resize(dF());
//		FPrevTurn.resize(dF());
//		FMotion.resize(dF());
//		FDirection.resize(dF());
//		FMax.resize(dF());
//		FMin.resize(dF());
//		GCurrent.resize(dF());
//		FatGCurrent.resize(dF());
//		GLastTurn.resize(dF());
//		GPrevTurn.resize(dF());
//		GMotion.resize(dF());
//		Bhhh.resize(dF());
//		Hess.resize(dF());
//		invHess.resize(dF());
//		invHessTemp.resize(dF());
//		
//		_initialize();
//	}
}

void sherpa::_initialize()
{
	FPrevTurn = ReadFCurrent();
	FLastTurn = ReadFCurrent();
	FBest = ReadFCurrent();
	if (isNan(ZBest)) ZBest = -INF;
	ZCurrent = -INF;
//	freshen();

}

void sherpa::free_memory()
{
	FCurrent.resize(0);
	_FCurrent_latest_objective.resize(0);
	FBest.resize(0);
	FLastTurn.resize(0);
	FPrevTurn.resize(0);
	FMotion.resize(0);
	FDirection.resize(0);
	FMax.resize(0);
	FMin.resize(0);
	GCurrent.resize(0);
	FatGCurrent.resize(0);
	GLastTurn.resize(0);
	GPrevTurn.resize(0);
	GMotion.resize(0);
	Bhhh.resize(0);
	Hess.resize(0);
	invHess.resize(0);
	invHessTemp.resize(0);	
}

string sherpa::printStatus(int which, double total_tol) const
{
	size_t pname_width = 9;
	for ( unsigned i=0; i<dF(); i++) {
		if (FNames[i].length() > pname_width) {
			pname_width = FNames[i].length();
		}
	}
	if (pname_width>60) pname_width = 60;

	ostringstream ret;
	if (which & status_FNames    ) {
		ret.width(pname_width);
		ret << std::left << "Parameter";
	}
	if (which & status_FCurrent       ) ret << "Current Value       ";
	if (which & status_FLastTurn      ) ret << "LastTurn            ";
	if (which & status_GCurrent       ) ret << "Current Gradient    ";
	if (which & status_FDirection     ) ret << "Direction           ";
	if (which & status_FDirectionLarge) ret << "Problematic";
	ret << "\n";

	for (unsigned i=0; i<dF(); i++) {
		ret << std::setprecision(6);
		if (which & status_FNames    ) {
			ret.width(pname_width);
			ret << std::left << FNames[i];
		}
		if (which & status_FCurrent  ) {
			ret.width(20);
			ret << ReadFCurrent()[i];
		}
		if (which & status_FLastTurn ) {
			ret.width(20);
			ret << FLastTurn[i];
		}
		if (which & status_GCurrent  ) {
			ret.width(20);
			ret << GCurrent[i];
		}
		if (which & status_FDirection) {
			ret.width(20);
			ret << FDirection[i];
		}
		if (which & status_FDirectionLarge) {
			ret.width( total_tol==0? 12:5);
			if (fabs(FDirection[i])>fabs(ReadFCurrent()[i])*100) {
				ret << "****";
			} else if (fabs(FDirection[i])>fabs(ReadFCurrent()[i])*10) {
				ret << "***";
			} else if (fabs(FDirection[i])>fabs(ReadFCurrent()[i])) {
				ret << "**";
			} else if (fabs(FDirection[i])>fabs(ReadFCurrent()[i])*0.1) {
				ret << "*";
			} else
			ret << "-";
			if (total_tol!=0) {
				ret.width( 7 );
				ret << std::fixed;
				ret << std::setprecision(4);
				ret << fabs(GCurrent[i]*FDirection[i])/fabs(total_tol);
				ret.unsetf(ios_base::floatfield);
			}
		}
		ret << "\n";
	}
	return ret.str();
}


sherpa::sherpa()
: weakself(nullptr)
, ParameterList()
, flag_gradient_diagnostic (0)
, flag_hessian_diagnostic (0)
, ZCurrent (NAN)
, ZBest (NAN)
, ZLastTurn (NAN)
, Bhhh ()
, Hess ()
, invHess ()
, invHessTemp ()
, robustCovariance ()
, FCurrent()
, _FCurrent_latest_objective()
, FBest()
, FLastTurn()
, FPrevTurn()
, FMotion()
, FDirection()
, FMax()
, FMin()
, GCurrent()
, GLastTurn()
, GPrevTurn()
, GMotion()
, FatGCurrent()
, max_iterations(1000)
, hessian_matrix()
{
	
}

sherpa::sherpa(const sherpa& dupe)
: weakself(nullptr)
, ParameterList(dupe)
, flag_gradient_diagnostic (0)
, flag_hessian_diagnostic (0)
, max_iterations(dupe.max_iterations)
{
	
}


//etk::symmetric_matrix* sherpa::covariance_matrix()
//{
//	return &invHess;
//}

etk::symmetric_matrix* sherpa::robust_covariance_matrix()
{
	return &robustCovariance;
}




bool sherpa::any_holdfast()
{
	for (size_t hi=0; hi<dF(); hi++) {
		if (FHoldfast.int8_at(hi)) {
			return true;
		}
	}
	return false;
}

size_t sherpa::count_holdfast()
{
	size_t n=0;
	for (size_t hi=0; hi<dF(); hi++) {
		if (FHoldfast.int8_at(hi)) {
			n++;
		}
	}
	return n;
}


void sherpa::hessfull_to_hessfree(const symmetric_matrix* full_matrix, symmetric_matrix* free_matrix)
{
	size_t hi, hj, fi, fj;
	fi=0;
	fj=0;
	for (hi=0; hi<dF(); hi++) {
		if (FHoldfast.int8_at(hi)) {
			// do not copy, this row is holdfast
		} else {
			fj=0;
			for (hj=0; hj<dF(); hj++) {
				if (FHoldfast.int8_at(hj)) {
					// do not copy, this column is holdfast
				} else {
					(*free_matrix)(fi,fj) = (*full_matrix)(hi,hj);
					fj++;
				}
			}
			fi++;
		}
	}
}


void sherpa::hessfree_to_hessfull(symmetric_matrix* full_matrix, const symmetric_matrix* free_matrix)
{
	size_t hi, hj, fi, fj;
	fi=0;
	fj=0;
	for (hi=0; hi<dF(); hi++) {
		if (FHoldfast.int8_at(hi)) {
			// do not copy, this row is holdfast
			for (hj=0; hj<dF(); hj++) {
				(*full_matrix)(hi,hj) = 0.0; //not NAN to avoid robust covariance problems;
			}
		} else {
			fj=0;
			for (hj=0; hj<dF(); hj++) {
				if (FHoldfast.int8_at(hj)) {
					// do not copy, this column is holdfast
					(*full_matrix)(hi,hj) = 0.0; //not NAN to avoid robust covariance problems;
				} else {
					(*full_matrix)(hi,hj) = (*free_matrix)(fi,fj);
					fj++;
				}
			}
			fi++;
		}
	}
}


std::string sherpa::ReadFCurrentAsString() const
{
	std::ostringstream ret;
	size_t s = FNames.size();
	if (s==0) {
		return "";
	}
	for (unsigned i=0; i<s; i++) {
		ret << "," << ReadFCurrent()[i];
	}
	return ret.str().substr(1);
}

std::string sherpa::ReadFLastTurnAsString() const
{
	std::ostringstream ret;
	size_t s = FNames.size();
	if (s==0) {
		return "";
	}
	for (unsigned i=0; i<s; i++) {
		ret << "," << ReadFLastTurn()[i];
	}
	return ret.str().substr(1);
}




etk::symmetric_matrix* sherpa::_get_inverse_hessian_array()
{

	if( (!invHess.pool) || (invHess.size()==0) ) {
		invHess.resize(dF(), dF());
		invHess.initialize(NAN);
	}

	return &invHess;
}

void sherpa::_set_inverse_hessian_array(etk::symmetric_matrix* in)
{
	invHess.same_memory_as(*in);
}

void sherpa::_del_inverse_hessian_array()
{
	Py_CLEAR(invHess.pool);
}




etk::symmetric_matrix* sherpa::_get_robust_covar_array()
{

	if( (!robustCovariance.pool) || (robustCovariance.size()==0) ) {
		robustCovariance.resize(dF(), dF());
		robustCovariance.initialize(NAN);
	}

	return &robustCovariance;
}

void sherpa::_set_robust_covar_array(etk::symmetric_matrix* in)
{
	robustCovariance.same_memory_as(*in);
}

void sherpa::_del_robust_covar_array()
{
	Py_CLEAR(robustCovariance.pool);
}






etk::ndarray* sherpa::_get_parameter_array(){
	return &FCurrent;
}

etk::ndarray* sherpa::_get_parameter_minbound_array(){
	return &FMin;
}

etk::ndarray* sherpa::_get_parameter_maxbound_array(){
	return &FMax;
}

etk::ndarray* sherpa::_get_holdfast_array(){
	return &FHoldfast;
}

etk::ndarray* sherpa::_get_null_values_array(){
	return &FNullValues;
}


etk::ndarray* sherpa::_get_init_values_array(){
	return &FInitValues;
}


void sherpa::_set_parameter_array(etk::ndarray* replacement){
	FCurrent.same_ccontig_memory_as(*replacement);
}

void sherpa::_set_parameter_minbound_array(etk::ndarray* replacement){
	FMin.same_ccontig_memory_as(*replacement);
}

void sherpa::_set_parameter_maxbound_array(etk::ndarray* replacement){
	FMax.same_ccontig_memory_as(*replacement);
}

void sherpa::_set_holdfast_array(etk::ndarray* replacement){
	if ( !replacement->pool || PyArray_DESCR(replacement->pool)->type_num != NPY_INT8) OOPS("assert failure, not NPY_INT8");
	FHoldfast.same_ccontig_memory_as(*replacement);
}

void sherpa::_set_null_values_array(etk::ndarray* replacement){
	FNullValues.same_ccontig_memory_as(*replacement);
}

void sherpa::_set_init_values_array(etk::ndarray* replacement){
	FInitValues.same_ccontig_memory_as(*replacement);
}




#include "larch_modelparameter.h"



elm::ModelParameter sherpa::parameter(const std::string& param_name,
								   const double& value,
								   const double& null_value,
								   const double& initial_value,
								   const double& max,
								   const double& min,
								   const int& holdfast,
								   const double& tstat)
{
	if (param_name=="") {
		throw(etk::ParameterNameError("Cannot name a parameter with an empty string."));
	}
	
	size_t prior_size = FNames.size();
	auto px = elm::ModelParameter(this, FNames.index_from_string(param_name));
	if (FNames.size() != prior_size) {
		tearDown();
		resize_allocated_memory();
	}
	
	if (FNames.size() > prior_size) {
		// set default things
		px._set_min(-INF);
		px._set_max(INF);
		px._set_value(0);
		px._set_nullvalue(0);
		px._set_initvalue(0);
	}
	
	if (!isNan(value)) {
		if (!isNan(px._get_initvalue())) {
			px._set_initvalue(value);
		}
		px._set_value(value);
	}
	if (!isNan(null_value)) {
		px._set_nullvalue(null_value);
	}
	if (!isNan(initial_value)) {
		px._set_initvalue(initial_value);
	}
	if (!isNan(min)) {
		px._set_min(min);
	}
	if (!isNan(max)) {
		px._set_max(max);
	}
	if (holdfast>=0) {
		px._set_holdfast((signed char)holdfast);
	}
	
	if (!isNan(tstat)) {
		px._set_t_stat(tstat);
	}
	
	return px;
}


#include "larch_modelparameter.h"


elm::ModelParameter sherpa::__getitem__(const std::string& param_name)
{
	if (FNames.has_key(param_name)) {
		return elm::ModelParameter(this, FNames[param_name]);
	}
	OOPS_KeyError("Parameter ",param_name," not found.");
}

elm::ModelParameter sherpa::__getitem__(const int& param_num)
{
	if (param_num > FNames.size()-1) OOPS_IndexError("Parameter number ",param_num," out of range (there are only ",FNames.size()," parameters)");
	if ((param_num < 0)) {
		//return parameter(FNames[FNames.size()+param_num]);
		if (-param_num <= int(FNames.size())) {
			return elm::ModelParameter(this, FNames.size()+param_num);
		}
		OOPS_IndexError("Parameter number ",param_num," out of range (there are only ",FNames.size()," parameters)");
	}
	return elm::ModelParameter(this, param_num);
}






freedom_alias& sherpa::alias_ref(const std::string& alias_name, const std::string& refers_to, const double& multiplier, const bool& force)
{
	if (alias_name=="") {
		throw(etk::ParameterNameError("Cannot name an alias with an empty string."));
	}
	if (refers_to=="") {
		throw(etk::ParameterNameError("Cannot refer to a parameter with an empty string."));
	}

	if (alias_name==refers_to) {
		throw(etk::ParameterNameError(etk::cat("Cannot create an alias '",refers_to,"' that refers to an existing parameter with the same name.")));
	}
	
	if (!force) {
		if (!FNames.has_key(refers_to) && (AliasInfo.find(refers_to)==AliasInfo.end())) {
			throw(etk::ParameterNameError(etk::cat("Cannot refer to parameter '",refers_to,"' that has not been previously defined.")));
		}

	}



	if (AliasInfo.find(alias_name)==AliasInfo.end()) {
		AliasInfo.emplace(alias_name,freedom_alias(alias_name, refers_to, multiplier));
	} else {
		AliasInfo.at(alias_name) = freedom_alias(alias_name, refers_to, multiplier);
	}

	
	return AliasInfo.at(alias_name);
}

elm::ModelAlias sherpa::alias(const std::string& alias_name, const std::string& refers_to, const double& multiplier, const bool& force)
{
	alias_ref(alias_name, refers_to, multiplier, force);
	return elm::ModelAlias(this, alias_name);
}


freedom_alias& sherpa::alias_ref(const std::string& alias_name)
{
	if (alias_name=="") {
		throw(etk::ParameterNameError("Cannot reference an alias with an empty string."));
	}

	if (AliasInfo.find(alias_name)==AliasInfo.end()) {
		throw(etk::ParameterNameError(etk::cat("Cannot find an alias named '",alias_name,"'.")));
	} else {
		return AliasInfo.at(alias_name);
	}
	
}

elm::ModelAlias sherpa::alias(const std::string& alias_name)
{
	alias_ref(alias_name);
	return elm::ModelAlias(this, alias_name);
}


void sherpa::del_alias(const std::string& alias_name)
{
	if (alias_name=="") {
		throw(etk::ParameterNameError("Cannot delete an alias named <empty string>."));
	}
	AliasInfo.erase(alias_name);
}

void sherpa::unlink_alias(const std::string& alias_name)
{
	if (alias_name=="") {
		throw(etk::ParameterNameError("Cannot unlink an alias named <empty string>."));
	}
	
	if (AliasInfo.find(alias_name)==AliasInfo.end()) {
		throw(etk::ParameterNameError("Cannot unlink an alias that does not exist."));
	}
	std::string refers_to = AliasInfo.at(alias_name).refers_to;
	double multiplier = AliasInfo.at(alias_name).multiplier;

	elm::ModelParameter px (this, FNames[refers_to]);
	double newval = px._get_value()*multiplier;
	double nullvalue = px._get_nullvalue()*multiplier;
	double initvalue = px._get_initvalue()*multiplier;
	double maxvalue = px._get_max()*multiplier;
	double minvalue = px._get_min()*multiplier;
	npy_int8 hold = px._get_holdfast();
	
	parameter(alias_name,
			  newval,
			  nullvalue,
			  initvalue,
			  maxvalue,
			  minvalue,
			  hold);
	
	AliasInfo.erase(alias_name);
}

