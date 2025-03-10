#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "common.h"
#include "state.h"
#include "condition.h"
#include "action.h"
#include "program_state.h"

class Instruction{
public:
	Instruction()= default;
	
	virtual ~Instruction()= default;
	
	// Maybe conds are not required, since only ZF and CF
	// are affected but already in a fix position
	virtual void addCond( Condition *c ){
		_conds.emplace_back( c );
	}
	
	virtual void setConds( vector< Condition* > &conds ){
		_conds = conds;
	}
	
	virtual bool isApplicable( ProgramState *ps ){
		return false;
	}
	
	virtual bool isGoalState( Instance *ins, ProgramState *ps ){
		return false;
	}
	
	virtual int apply( ProgramState *ps ){ return 0; }

	virtual string getSchema() const{ return "UNDEFINED"; }

	virtual string getName() const{ return "UNDEFINED"; }
	
	virtual string toString( bool full_info ) const{ return "[INSTRUCTION] EMPTY\n"; }

	virtual bool operator==(const Instruction *& other) {

		return this->getSchema() == other->getSchema();
	}
protected:
	vector< Condition* > _conds;
};

template<> struct std::hash<Instruction *> {
	std::size_t operator()(Instruction * const& inst) const noexcept {
        return std::hash<string>()(inst->getSchema()); // or use boost::hash_combine (see Discussion) https://en.cppreference.com/w/Talk:cpp/utility/hash
    }
};

template<> struct std::hash<Instruction> {
	std::size_t operator()(Instruction const& inst) const noexcept {
        return std::hash<string>()(inst.getSchema()); // or use boost::hash_combine (see Discussion) https://en.cppreference.com/w/Talk:cpp/utility/hash
    }
};


class PlanningAction : public Instruction{
public:
	explicit PlanningAction( Action *act ){
		_act = act;
	}
	
	~PlanningAction() override = default;
	
	bool isApplicable( ProgramState *ps ) override{
		State *s = ps->getState();
		return _act->isApplicable( s );
	}
	
	int apply( ProgramState *ps ) override{
		State *s = ps->getState();
		int line = ps->getLine();
		
		// State is updated in action function
		//int res = _act->apply( s );

		// Conditional effects
		int res = 0;
		if( isApplicable( ps ) ){
		    res = _act->apply( s );
		}
		
		ps->setLine( line + 1 );
		
		return res;
	}

	virtual string getSchema() const override{ 
		return _act->getSchema(); 
	}
	
	virtual string getName() const override{
		return _act->getName();
	}
	
	virtual string getType() const{
		return _act->getType();
	}
	
	string toString( bool full_info ) const override{
		if( full_info )	
			return "[INSTRUCTION][PLANNING ACTION]:\n" + _act->toString(true);
		return _act->toString( full_info );
	}
	
private:
	Action *_act;
};

class RAMAction : public PlanningAction {
	using PlanningAction::PlanningAction;
};

class Goto : public Instruction{
public:
	explicit Goto( int dest = 0 ){
		_dest_line = dest;
	}
	
	~Goto() override = default;
	
	int getDestinationLine() const{
		return _dest_line;
	}
	
	int apply( ProgramState *ps ) override{
		State *s = ps->getState();
		int line = ps->getLine();
		
		bool evals_true = true;
		for( int i = 0; evals_true && i < int( _conds.size() ); i++ ){
			evals_true = _conds[ i ]->evalCondition( s );
		}
		
		if( evals_true ) ps->setLine( line + 1 );
		else ps->setLine( _dest_line );
		
		return 0;
	}

	virtual string getSchema() const override{ 
		return "GOTO"; 
	}

	virtual string getName() const override{
		return getSchema();
	}
	
	string toString( bool full_info ) const override{
		string ret;
		if( full_info ){
			ret = "[INSTRUCTION][GOTO]:\n"; 
			ret += "DESTINATION LINE: " + to_string( _dest_line ) + "\n";
		}
		else{
			ret = "goto(" + to_string( _dest_line ) + ",!(";
		}
		
		for(auto cond : _conds)
			ret += cond->toString( full_info );
			
		if( !full_info ) ret += "))\n";
		return ret;
	}
	
private:
	int _dest_line;
};

class End : public Instruction{
public:
	End()= default;
	
	~End() override =default;
	
	/*
		since isGoalState is independent from instruction, 
		it will be moved to instance.h
	*/

	// bool isGoalState( Instance *ins, ProgramState *ps ) override{
	// 	State *s = ps->getState();
	// 	vector< Condition* > goal = ins->getGoalCondition();
		
	// 	for(auto & g : goal){
	// 		if( !g->evalCondition( s ) )
	// 			return false;
	// 	}
		
	// 	return true;
	// }

	virtual string getSchema() const override{ 
		return "END";
	}

	virtual string getName() const override{
		return getName(); 
	}
	
	string toString( bool full_info ) const override{
		string ret = "end\n";
		if( full_info ){
			ret= "[INSTRUCTION][END]:\n";
			for(auto cond : _conds)
				if( cond )
					ret += cond->toString(true);
		}
		return ret;
	}
	
};

#endif
