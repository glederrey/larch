/*
 *  elm_darray.cpp
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

#include <iostream>

#include <cstring>
#include "etk.h"
#include "etk_refcount.h"
//#include "elm_sql_facet.h"
#include "elm_darray.h"
#include "elm_cellcode.h"
#include "elm_vascular.h"

using namespace std;
using namespace etk;


void elm::darray_req::set_variables( const std::vector<std::string>& varnames )
{
	variables.clear();
	variables_map.clear();

	std::vector<std::string>::const_iterator x;
	int i=0;
	for (x = varnames.begin(); x != varnames.end(); x++) {
		variables.push_back(*x);
		variables_map[*x] = i;
		i++;
	}
}

const std::vector<std::string>& elm::darray_req::get_variables() const
{
	return variables;
}











std::string elm::darray_req::__str__() const
{
	std::ostringstream s;
	s << "larch.darray_req";
	
	if (dimty==3) {
		s << " [cav]";
	} else if (dimty==2) {
		s << " [cv]";
	} else {
		s << " [?dimty="<<dimty<<"?]";
	}
	
	if (dtype==NPY_DOUBLE) {
		s << " dtype=double";
	} else if (dtype==NPY_INT64) {
		s << " dtype=int64";
	} else if (dtype==NPY_BOOL) {
		s << " dtype=bool";
	} else {
		s << " dtype=?";
	}

	auto v=variables.begin();
	if (v!=variables.end()) {
		s << " {";
		s << *v;
		v++;
		while (v!=variables.end()) {
			s << ","<<*v;
			v++;
		}
		s << "}";
	}
	
	s << "";
	return s.str();
}

std::string elm::darray_req::__repr__() const
{
	std::ostringstream s;
	s << "<larch.darray_req";
	
	if (dimty==3) {
		s << " [cav]";
	} else if (dimty==2) {
		s << " [cv]";
	} else {
		s << " [?dimty="<<dimty<<"?]";
	}
	
	if (dtype==NPY_DOUBLE) {
		s << " dtype=double";
	} else if (dtype==NPY_INT64) {
		s << " dtype=int64";
	} else if (dtype==NPY_BOOL) {
		s << " dtype=bool";
	} else {
		s << " dtype=?";
	}
	
	s << " (";
	try {
		s << nVars();
	} SPOO {
		s << "?";
	}
	s << " vars)";
	
	s << ">";
	return s.str();
}


elm::darray_req::darray_req(int dim, int tp, int nalts)
: dimty          (dim)
, dtype          (tp)
, variables      ()
, n_alts         (nalts)
, contig         (true)
{
}

elm::darray_req::darray_req(const elm::darray_req& x)
: dimty          (x.dimty)
, dtype          (x.dtype)
, variables      (x.variables)
, n_alts         (x.n_alts)
, contig         (x.contig)
{
}

elm::darray_req::darray_req()
: dimty          (2)
, dtype          (NPY_DOUBLE)
, variables      ()
, n_alts         (0)
, contig         (true)
{
}


elm::darray_req::~darray_req()
{
}

int elm::darray_req::satisfied_by(const elm::darray* x) const
{
	if (x->dimty != dimty) return -1;
	if (x->dtype != dtype) return -2;
	if ((x->get_variables().size()>0) && (variables.size()>0) && (x->get_variables() != variables)) return -3;
	if (!contig) return 0;
	if (contig && x->contig) return 0;
	return -4;
}

size_t elm::darray_req::nVars() const
{
	return variables.size();
}

size_t elm::darray::nVars() const
{
	if (dimty==2) {
		return _repository.size2();
	}
	if (dimty==3) {
		return _repository.size3();
	}
	return 1;
}

size_t elm::darray_req::nAlts() const
{
	return n_alts;
}

size_t elm::darray::nCases() const
{
	return _repository.size1();
}


elm::darray::darray()
: elm::darray_req()
, _repository()
{
}

elm::darray::darray(const elm::darray& source_arr)
: elm::darray_req()
, _repository(source_arr._repository, true)
{
}

elm::darray::darray(const elm::darray& source_arr, double scale)
: elm::darray_req()
, _repository(source_arr._repository.size1(),source_arr._repository.size2(),source_arr._repository.size3())
{
	cblas_daxpy(_repository.size(), scale, source_arr._repository.ptr(), 1, _repository.ptr(), 1);
}

elm::darray::darray(PyObject* source_arr)
: elm::darray_req()
, _repository(source_arr)
{
	if (!PyArray_Check(source_arr)) {
		OOPS("input must be an array");
	}

	etk::strvec vs;
	if (PyObject_HasAttrString(source_arr, "vars")) {
		PyObject* py_vars = PyObject_GetAttrString(source_arr, "vars");
		
		for (Py_ssize_t i = 0; i<PySequence_Size(py_vars); i++) {
			PyObject* item = PySequence_GetItem(py_vars, i);
			if (!item) {
				OOPS("failed reading var name");
			}
			vs.push_back(PyString_ExtractCppString(item));
			Py_CLEAR(item);
		}
		
		Py_CLEAR(py_vars);
	}
	
	int dim = PyArray_NDIM((PyArrayObject*) source_arr);
	dimty = dim;
	dtype = PyArray_TYPE((PyArrayObject*) source_arr);
	contig = PyArray_ISCARRAY((PyArrayObject*) source_arr);
	
	if (dim==3) {
		if (vs.size()>0 && vs.size()!=_repository.size3()) {
			OOPS("input array does not have correct number of vars defined (",vs.size()," names for ",_repository.size3()," numbers)");
		}
		n_alts = PyArray_DIMS((PyArrayObject*) source_arr)[1];
	} else if (dim==2) {
		if (vs.size()>0 && vs.size()!=_repository.size2()) {
			OOPS("input array does not have correct number of vars defined (",vs.size()," names for ",_repository.size2()," numbers)");
		}
	} else {
		OOPS("input array must have 2 (case-var) or 3 (case-alt-var) dimensions, this array has ",dim);
	}
	
	
	set_variables(vs);
}




elm::darray::darray(int dtype, int ncases, int nalts, int nvars)
: elm::darray_req(3,dtype,nalts)
, _repository("Array",dtype,ncases,nalts,nvars)
{
}

elm::darray::darray(int dtype, int ncases, int nvars)
: elm::darray_req(2,dtype)
, _repository("Array",dtype,ncases,nvars)
{
}

elm::darray::darray(int dtype, int ncases)
: elm::darray_req(1,dtype)
, _repository("Array",dtype,ncases)
{
}




elm::darray::~darray()
{
	_repository.destroy();
}




etk::ptr_lockout<const double> elm::darray::values(const unsigned& firstcasenum, const size_t& numberofcases)
{
	return ptr_lockout<const double>(_repository.ptr(firstcasenum), _repo_lock);
}

etk::ptr_lockout<const bool> elm::darray::boolvalues(const unsigned& firstcasenum, const size_t& numberofcases)
{
	return ptr_lockout<const bool>(_repository.ptr_bool(firstcasenum), _repo_lock);
}

etk::ptr_lockout<const double> elm::darray::values(const unsigned& firstcasenum, const size_t& numberofcases) const
{
	return ptr_lockout<const double>(_repository.ptr(firstcasenum), const_cast<elm::darray*>(this)->_repo_lock);
}

etk::ptr_lockout<const bool> elm::darray::boolvalues(const unsigned& firstcasenum, const size_t& numberofcases) const
{
	return ptr_lockout<const bool>(_repository.ptr_bool(firstcasenum), const_cast<elm::darray*>(this)->_repo_lock);
}

const double* elm::darray::values_constptr(const unsigned& firstcasenum) const
{
	return _repository.ptr(firstcasenum);
}

const bool* elm::darray::boolvalues_constptr(const unsigned& firstcasenum) const
{
	return _repository.ptr_bool(firstcasenum);
}


double elm::darray::value(const unsigned& c, const unsigned& a, const unsigned& v) const
{
	return *(values(c,1)+(a*nVars())+v);
}

double elm::darray::value(const unsigned& c, const unsigned& v) const
{
	return *(values(c,1)+v);
}

bool elm::darray::boolvalue(const unsigned& c, const unsigned& a, const unsigned& v) const
{
	return *(boolvalues(c,1)+(a*nVars())+v);
}
bool elm::darray::boolvalue(const unsigned& c, const unsigned& v) const
{
	return *(boolvalues(c,1)+v);
}


const double& elm::darray::value_double    (const size_t& c, const size_t& a, const size_t& v) const 
{
	return *_repository.ptr(c,a,v);
}
const double& elm::darray::value_double    (const size_t& c, const size_t& v) const
{
	return *_repository.ptr(c,v);
}



double& elm::darray::value_double    (const size_t& c, const size_t& a, const size_t& v)
{
	return *_repository.ptr(c,a,v);
}
double& elm::darray::value_double    (const size_t& c, const size_t& v)
{
	return *_repository.ptr(c,v);
}
long long& elm::darray::value_int64    (const size_t& c, const size_t& a, const size_t& v)
{
	return *(long long*)_repository.voidptr(c,a,v);
}
long long& elm::darray::value_int64    (const size_t& c, const size_t& v)
{
	return *(long long*)_repository.voidptr(c,v);
}

bool& elm::darray::value_bool    (const size_t& c, const size_t& a, const size_t& v)
{
	return *(bool*)_repository.voidptr(c,a,v);
}
bool& elm::darray::value_bool    (const size_t& c, const size_t& v)
{
	return *(bool*)_repository.voidptr(c,v);
}



std::string elm::darray::printcase(const unsigned& r) const
{
	std::ostringstream ret;
	unsigned x2, x3;
	char depMarker, colMarker, rowMarker;
	ret << "["<< nVars() <<" vars, "<<dimty<<" dims]";
	if (dimty==2 && dtype==NPY_DOUBLE) {
		depMarker = ' ';
		colMarker = '\t';
		rowMarker = '\n';
		for ( x2=0; x2<nVars(); x2++ ) { 
			ret << value(r,x2) << colMarker;
		}
		ret << rowMarker;
	} else if (dimty==3 && dtype==NPY_DOUBLE) {
		depMarker = '\t';
		colMarker = '\n';
		rowMarker = '\n';
		for ( x2=0; x2<nAlts(); x2++ ) { 
			for ( x3=0; x3<nVars(); x3++ ) { 
				ret << value(r,x2,x3) << depMarker;
			}
			ret << colMarker;
		}
		ret << rowMarker;
	}
	return ret.str();
}

std::string elm::darray::printcases(unsigned rstart, const unsigned& rfinish) const
{
	std::ostringstream ret;
	for (; rstart < rfinish; rstart++) {
		ret << printcase(rstart);
	}
	return ret.str();
}


std::string elm::darray::printboolcase(const unsigned& r) const
{
	std::ostringstream ret;
	unsigned x2, x3;
	char depMarker, colMarker, rowMarker;
	if (dimty==2 && dtype==NPY_BOOL) {
		depMarker = ' ';
		colMarker = '\t';
		rowMarker = '\n';
		for ( x2=0; x2<nVars(); x2++ ) { 
			ret << boolvalue(r,x2) << colMarker;
		}
		ret << rowMarker;
	} else if (dimty==3 && dtype==NPY_BOOL) {
		depMarker = '\t';
		colMarker = '\n';
		rowMarker = '\n';
		for ( x2=0; x2<nAlts(); x2++ ) { 
			for ( x3=0; x3<nVars(); x3++ ) { 
				ret << boolvalue(r,x2,x3) << depMarker;
			}
			ret << colMarker;
		}
		ret << rowMarker;
	}
	return ret.str();
}

std::string elm::darray::printboolcases(unsigned rstart, const unsigned& rfinish) const
{
	std::ostringstream ret;
	for (; rstart < rfinish; rstart++) {
		ret << printboolcase(rstart);
	}
	return ret.str();
}





void elm::darray::ExportData (double* ExportTo, const unsigned& c, const unsigned& a, const unsigned& numberOfAlts) const
{
	if (dimty==3) {
		cblas_dcopy(nVars(),values(c,1)+(a*nVars()),1,ExportTo,1);
		ExportTo+=nVars();  
	} else if (dimty==2) {
		memset(ExportTo,0,nVars()*numberOfAlts*sizeof(double));
		cblas_dcopy(nVars(),values(c,1),1,ExportTo+a,numberOfAlts);
	}
}
void elm::darray::ExportData	(double* ExportTo, const unsigned& c, const unsigned& a, const double& scale, const unsigned& numberOfAlts) const
{
	if (dimty==3) {
		cblas_dcopy(nVars(),values(c,1)+(a*nVars()),1,ExportTo,1);
		cblas_dscal(nVars(), scale, ExportTo,1);
		ExportTo+=nVars();
	} else if (dimty==2) {
		memset(ExportTo,0,(nVars()*numberOfAlts)*sizeof(double));
		cblas_dcopy(nVars(),values(c,1),1,ExportTo+a,numberOfAlts);
		cblas_dscal(nVars(), scale, ExportTo+a,numberOfAlts);
		ExportTo+=(nVars()*numberOfAlts);
	}
}

void elm::darray::OverlayData(double* ExportTo, const unsigned& c, const unsigned& a, const double& scale, const unsigned& numberOfAlts) const
{
	if (dimty==3) {
		cblas_daxpy(nVars(),scale,values(c,1)+(a*nVars()),1,ExportTo,1);
		ExportTo+=nVars();
	} else if (dimty==2) {
		cblas_daxpy(nVars(),scale,values(c,1),1,ExportTo+a,numberOfAlts);
		ExportTo+=(nVars()*numberOfAlts);
	}		
}

PyObject* elm::darray::get_array()
{
	PyObject* x = _repository.get_object();
	return x;
}



std::string elm::darray::__str__() const
{
	std::ostringstream s;
	s << "larch.darray";
	
	if (dimty==3) {
		s << " [cav]";
	} else if (dimty==2) {
		s << " [cv]";
	} else {
		s << " [?dimty="<<dimty<<"?]";
	}
	
	if (dtype==NPY_DOUBLE) {
		s << " dtype=double";
	} else if (dtype==NPY_INT64) {
		s << " dtype=int64";
	} else if (dtype==NPY_BOOL) {
		s << " dtype=bool";
	} else {
		s << " dtype=?";
	}

	auto v=variables.begin();
	if (v!=variables.end()) {
		s << " {";
		s << *v;
		v++;
		while (v!=variables.end()) {
			s << ","<<*v;
			v++;
		}
		s << "}";
	}
	
	s << "";
	return s.str();
}

std::string elm::darray::__repr__() const
{
	std::ostringstream s;
	s << "<larch.darray";
	
	if (dimty==3) {
		s << " [cav]";
	} else if (dimty==2) {
		s << " [cv]";
	} else {
		s << " [?dimty="<<dimty<<"?]";
	}
	
	if (dtype==NPY_DOUBLE) {
		s << " dtype=double";
	} else if (dtype==NPY_INT64) {
		s << " dtype=int64";
	} else if (dtype==NPY_BOOL) {
		s << " dtype=bool";
	} else {
		s << " dtype=?";
	}
	
	s << " (";
	try {
		s << nVars();
	} SPOO {
		s << "?";
	}
	s << " vars)";
	
	s << ">";
	return s.str();
}


std::string elm::check_darray(const darray* x)
{
	std::ostringstream s;
	s << "check larch.darray";
	
	if (x->dimty==3) {
		s << " [cav]";
	} else if (x->dimty==2) {
		s << " [cv]";
	} else {
		s << " [?dimty="<<x->dimty<<"?]";
	}
	
	if (x->dtype==NPY_DOUBLE) {
		s << " dtype=double";
	} else if (x->dtype==NPY_INT64) {
		s << " dtype=int64";
	} else if (x->dtype==NPY_BOOL) {
		s << " dtype=bool";
	} else {
		s << " dtype=?";
	}

	auto v=x->get_variables().begin();
	if (v!=x->get_variables().end()) {
		s << " {";
		s << *v;
		v++;
		while (v!=x->get_variables().end()) {
			s << ","<<*v;
			v++;
		}
		s << "}";
	}
	
	s << "";
	return s.str();

}














elm::darray_export_map::darray_export_map(etk::ndarray* caseindexes, etk::ndarray* altindexes, etk::ndarray* data_array, const size_t& n_cases, const size_t& n_alts)
: _pointer_map()
, _data_array(nullptr)
, _caseindexes(nullptr)
, _altindexes(nullptr)
, n_cases(n_cases)
, n_alts(n_alts)
{
	maplink(caseindexes, altindexes, data_array, n_cases, n_alts);
}



elm::darray_export_map::~darray_export_map()
{
	clear();
}


void elm::darray_export_map::maplink(etk::ndarray* caseindexes, etk::ndarray* altindexes, etk::ndarray* data_array, const size_t& n_cases, const size_t& n_alts)
{
	clear();
	if (caseindexes && altindexes && data_array) {
		this->n_cases = n_cases;
		this->n_alts = n_alts;
		_data_array = std::make_shared<etk::ndarray>(*data_array, true);
		_caseindexes = std::make_shared<etk::ndarray>(*caseindexes, true);
		_altindexes = std::make_shared<etk::ndarray>(*altindexes, true);
		
		_casestarts = std::make_shared<etk::ndarray>("Array", NPY_INT64, n_cases);
		_casestarts->int64_initialize(-1);
		
		size_t caseindexticker = 0;
		_casestarts->int64_at(caseindexticker) = 0;
		
		for (size_t rowticker=0; rowticker<data_array->size1(); rowticker++) {
			_pointer_map[two_int64(caseindexes->int64_at(rowticker),altindexes->int64_at(rowticker))] = _data_array->ptr(rowticker);
			
			while (caseindexticker<caseindexes->int64_at(rowticker)) {
				caseindexticker++;
				_casestarts->int64_at(caseindexticker) = rowticker;
			}
		}
	}
}

void elm::darray_export_map::clear()
{
	_pointer_map.clear();
	_data_array .reset();
	_caseindexes .reset();
	_altindexes .reset();
	n_cases=0;
	n_alts=0;
}


const double* elm::darray_export_map::get_ptr_at(const long long& caseindex, const long long& altindex) const
{
	auto iter = _pointer_map.find(two_int64(caseindex,altindex));
	if (iter==_pointer_map.end()) {
		return nullptr;
	}
	return iter->second;
}


void elm::darray_export_map::export_into (double* ExportTo, const unsigned& c, const unsigned& a, const unsigned& numberOfVars) const
{
	const double* ptr = get_ptr_at(c, a);
	if (ptr) {
		cblas_dcopy(numberOfVars, ptr, 1, ExportTo, 1);
	}
}



double elm::darray_export_map::get_value_at(const long long& caseindex, const long long& altindex, const long long& varindex) const
{
	const double* ptr = get_ptr_at(caseindex, altindex);
	if (ptr) {
		return ptr[varindex];
	}
	return NAN;
}


