/*
 *  elm_model2_choice.cpp
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


#include <cstring>
#include "elm_model2.h"
#include "elm_queryset.h"

//void elm::Model2::simulate_choices
//( const std::string& tablename
//, const std::string& choicecolumnname
//, const std::string& casecolumnname
//, const std::string& altcolumnname
//, const std::string& choices_per_case_columnname
//, bool overwrite
//)
//{
//	setUp();
//	_simulate_choices( tablename, choicecolumnname, casecolumnname, altcolumnname, choices_per_case_columnname, overwrite);
//	tearDown();
//}



//void elm::Model2::_simulate_choices
//( const std::string& tablename
//, const std::string& choicecolumnname
//, const std::string& casecolumnname
//, const std::string& altcolumnname
//, const std::string& choices_per_case_columnname
//, bool overwrite
//)
//{
//	if (!_Data) OOPS("A database must be linked to this model to do this.");
//	std::ostringstream sql;
//	
//	// Calculate the probabilities
//	calculate_probability();
//	
//	// Read the raw casenumbers from elm_case_ids
//	std::vector<elm::caseid_t> case_ids (_Data->caseids());
//	
//	// Drop the old table if it exists and is to be overwritten
//	if (overwrite) {
//		sql << "DROP TABLE IF EXISTS " << tablename;
//		_Data->sql_statement(sql)->execute_until_done();
//		sql.str(""); sql.clear();
//		sql << "DROP VIEW IF EXISTS " << tablename;
//		_Data->sql_statement(sql)->execute_until_done();
//		sql.str(""); sql.clear();
//	} else {
//		etk::strvec alltablenames (_Data->all_table_names());
//		if (alltablenames.contains(tablename)) OOPS("The table ",tablename," already exists in the database.");
//	}
//	
//	// Create a new table to hold the probabilities
//	sql << "CREATE TABLE " << tablename << " (" ;
//	sql << casecolumnname << " INTEGER, ";
//	sql << altcolumnname << " INTEGER, ";
//	sql << choicecolumnname << " DOUBLE ";
//	sql << ")";
//	_Data->sql_statement(sql)->execute_until_done();
//	sql.str(""); sql.clear();
//
//	// Write casenumbers and probabilities to file
//	sql << "INSERT INTO " << tablename << " VALUES (?,?,?);";
//	SQLiteStmtPtr s= _Data->sql_statement(sql.str());
//	
//	etk::randomizer rndm;
//	double q;
//	
//	for (unsigned i=0; i<_Data->nCases(); i++) {
//		if (choices_per_case_columnname!="") {
//			std::ostringstream wt;
//			wt << "SELECT "<<choices_per_case_columnname<<" FROM "+_Data->tbl_idco()+" WHERE caseid="<<case_ids[i];
//			int choices = _Data->sql_statement(wt)->execute()->getInt(0);
//			etk::dblvec choice_indicate;
//			choice_indicate.resize(_Data->nAlts(), 0.0);
//			for (int r=0; r<choices; r++) {
//				q = rndm.uniform();
//				for (unsigned a=0; a<_Data->nAlts(); a++) {
//					q -= Probability(i,a);
//					if (q<0) {
//						choice_indicate[a] += 1; // chosen
//						break;
//					} 
//				}
//			}
//			for (unsigned a=0; a<_Data->nAlts(); a++) {
//				if (!Data_Avail->boolvalue(i,a)) { s->reset(); continue; }
//				s->bind_int64(1, case_ids[i]);
//				s->bind_int64(2, Xylem[a]->code());
//				s->bind_double(3, choice_indicate[a]);
//				BUGGER(msg) << "simulate_choices:"<<case_ids[i]<<":"<<Xylem[a]->code()<<":"<<choice_indicate[a];
//				s->execute_until_done();
//				s->reset();
//			}
//		} else {
//			q = rndm.uniform();
//			for (unsigned a=0; a<_Data->nAlts(); a++) {
//				q -= Probability(i,a);
//				s->bind_int64(1, case_ids[i]);
//				s->bind_int64(2, Xylem[a]->code());
//				if (q<0) {
//					s->bind_double(3, 1); // chosen
//					q += 100;
//				} else {
//					s->bind_double(3, 0); // not chosen
//				}
//				s->execute();
//				s->reset();
//			}
//		}
//	}
//}



//std::string elm::Model2::choice(const std::string& choicevarname)
//{
//	std::ostringstream ret;
//	if (choicevarname.empty()) {
//		choseness_CA_variable.clear();
//		ret << "success: chosen_CA is cleared";
//	} else {
//		choseness_CA_variable = choicevarname;
//		ret << "success: choice is set to the idCA variable " << choicevarname;
//	}
////	if (_is_setUp>=2) {
////		_setUp_choice_data();
////	}
//	return ret.str();
//}

PyObject*	elm::Model2::_get_choice() const
{
	return etk::py_one_item_list(PyString_FromString(choseness_CA_variable.c_str())); 
}










