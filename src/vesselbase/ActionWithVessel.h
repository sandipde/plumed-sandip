/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Copyright (c) 2012 The plumed team
   (see the PEOPLE file at the root of the distribution for a list of names)

   See http://www.plumed-code.org for more information.

   This file is part of plumed, version 2.0.

   plumed is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   plumed is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with plumed.  If not, see <http://www.gnu.org/licenses/>.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
#ifndef __PLUMED_vesselbase_ActionWithVessel_h
#define __PLUMED_vesselbase_ActionWithVessel_h

#include "core/ActionWithValue.h"
#include "core/ActionAtomistic.h"
#include "tools/Exception.h"
#include <vector>

namespace PLMD{
class Value;

namespace vesselbase{

class Vessel;

/**
\ingroup MULTIINHERIT
This is used to create PLMD::Action objects that are computed by calculating the same function multiple
times.  This is used in PLMD::MultiColvar.
*/

class ActionWithVessel : public virtual Action {
friend class Vessel;
friend class FieldVessel;
private:
/// This is used to ensure that we have properly read the action
  bool read;
/// Do all calculations in serial
  bool serial;
/// The tolerance on the accumulators for neighbour list
  double tolerance;
/// The value of the current element in the sum
  double thisval;
/// Vector of derivatives for the object
  std::vector<double> derivatives;
/// The buffers we use for mpi summing DistributionFunction objects
  std::vector<double> buffer;
/// Pointers to the functions we are using on each value
  std::vector<Vessel*> functions;
/// Avoid hiding of base class virtual function
  using Action::deactivate;
protected:
/// Add a vessel to the list of vessels
  void addVessel( const std::string& name, const std::string& input, const unsigned numlab=0 );
/// Complete the setup of this object (this routine must be called after construction of ActionWithValue)
  void readVesselKeywords();
/// Return the value of the tolerance
  double getTolerance() const ;
/// Get the number of vessels
  unsigned getNumberOfVessels() const;
/// Get a pointer to the ith vessel
   Vessel* getPntrToVessel( const unsigned& i );
/// Calculate the values of all the vessels
  void runAllTasks( const unsigned& ntasks );
/// Resize all the functions when the number of derivatives change
  void resizeFunctions();
/// Clear all the data from the buffers 
  void clearBuffers();
/// Set the value of the element
  void setElementValue( const double& );
///  Add some derivative of the quantity in the sum wrt to a numbered element
  void addElementDerivative( const unsigned&, const double& );
public:
  static void registerKeywords(Keywords& keys);
/// By calling this function during register keywords you tell plumed to use a
/// a default method to parallelize the calculation.
  static void autoParallelize(Keywords& keys);
  ActionWithVessel(const ActionOptions&ao);
  ~ActionWithVessel();
/// Activate the jth colvar
/// Deactivate the current task in future loops
  virtual void deactivate_task()=0;
/// Merge the derivatives
  virtual void chainRuleForElementDerivatives( const unsigned j, const unsigned& vstart, const double& df, Vessel* valout );
  virtual void transferDerivatives( const unsigned j, const Value& value_in, const double& df, Value* valout );
/// Can we skip the calculations of quantities
  virtual bool isPossibleToSkip(); 
/// Are the base quantities periodic
  virtual bool isPeriodic()=0;
/// What are the domains of the base quantities
  virtual void retrieveDomain( std::string& min, std::string& max);
/// Get the number of functions from which we are calculating the distribtuion
  virtual unsigned getNumberOfFunctionsInAction()=0;
/// Get the number of derivatives for final calculated quantity 
  virtual unsigned getNumberOfDerivatives()=0;
/// Get number of derivatives for ith function
  virtual unsigned getNumberOfDerivatives( const unsigned& i );
/// Calculate one of the functions in the distribution
  virtual bool performTask( const unsigned& j )=0;
/// Return a pointer to the field 
  Vessel* getVessel( const std::string& name );
/// Get the value of this element
  double getElementValue() const ;
/// Retrieve the derivative of the quantity in the sum wrt to a numbered element
  double getElementDerivative( const unsigned& ) const ;
};

inline
double ActionWithVessel::getTolerance() const {
  return tolerance;
}

inline
bool ActionWithVessel::isPossibleToSkip(){
  return false;
}

inline
unsigned ActionWithVessel::getNumberOfVessels() const {
  return functions.size();
}

inline
Vessel* ActionWithVessel::getPntrToVessel( const unsigned& i ){
  plumed_dbg_assert( i<functions.size() );
  return functions[i];
}

inline
double ActionWithVessel::getElementValue() const {
  return thisval;
}

inline
void ActionWithVessel::setElementValue( const double& val ){
  thisval=val;
}

inline
void ActionWithVessel::clearBuffers(){
  buffer.assign(buffer.size(),0.0);
}

inline
unsigned ActionWithVessel::getNumberOfDerivatives( const unsigned& i ){
  return derivatives.size();
}

inline
double ActionWithVessel::getElementDerivative( const unsigned& ider ) const {
  plumed_dbg_assert( ider<derivatives.size() );
  return derivatives[ider];
}

inline
void ActionWithVessel::addElementDerivative( const unsigned& ider, const double& der ){
  plumed_dbg_assert( ider<derivatives.size() );
  derivatives[ider] += der;
}

} 
}
#endif