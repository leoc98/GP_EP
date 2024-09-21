#ifndef __STATE_H__
#define __STATE_H__

#include "common.h"
#include "state_descriptor.h"
#include "state_type.h"
#include "EpistemicModel/epistemic_model.h"
#include "EpistemicModel/epistemic_variable.h"

class State{
public:
    using predicate = StateType::predicate;
    using predicates = StateType::predicates;    
    // Default constructor
    // State(){
    //     _instance_id = 0;
    // }

    // Constructor mainly used for initial states
	explicit State( StateDescriptor *sd, int instance_id = 0 ):
            // initial global state
            _typed_epistemic(1), 
            _epistemic_string_to_id({{"", 0}}),
            _epistemic_history({{_typed_epistemic[0],{}}}), 
            _typed_registers(_epistemic_history.find(_typed_epistemic[0])->second)
    {
        auto var_types = sd->getVarTypes();
        _typed_pointers.resize( var_types.size() );
        for( int i = 0; i < int( var_types.size() ); i++ ){
            auto num_typed_pointers = sd->getNumTypedPointers( var_types[i] );
            _typed_pointers[i].resize( num_typed_pointers, 0 );
        }

        auto pred_types = sd->getPredicateTypes();
        _typed_registers.push_back( predicates( pred_types.size() ) );
        // _typed_registers.back().resize( pred_types.size() );
        _instance_id = instance_id;
	}

	// Constructor mainly used to make copies and propagate effects
	explicit State( State *s ):
            _typed_epistemic(s->_typed_epistemic),
            _epistemic_string_to_id(s->_epistemic_string_to_id),
            _epistemic_history(s->_epistemic_history),
            _typed_registers(_epistemic_history.find(_typed_epistemic[0])->second),
            _lastest_history_index(s->_lastest_history_index)
    {
		_typed_pointers = s->getTypedPointers();
        _instance_id = s->getInstanceID();
	}
	
	State* copy() {
		return new State( this );
	}

	const vector< vector< int > >& getTypedPointers() const{
	    return _typed_pointers;
	}


	void setPointersSize( unsigned p_size ){
        _typed_pointers.resize( p_size );
    }

    void addPointer( StateDescriptor *sd, const string &var_type, const string &pointer_name ){
        bool add = sd->addPointer( var_type, pointer_name );
        assert( add );
        auto var_idx = sd->getVarIDX( var_type );
        assert( var_idx < (int)_typed_pointers.size() );
        _typed_pointers[ var_idx ].resize( sd->getNumTypedPointers( var_type ) );
    }

	void setPointer( StateDescriptor *sd, const string &var_type, int idx, int value ){
        auto var_idx = sd->getVarIDX( var_type );
        assert( var_idx < (int)_typed_pointers.size() );
        assert( idx < (int)_typed_pointers[ var_idx ].size() );
        _typed_pointers[ var_idx ][ idx ] = value;
    }

    void setPointer( StateDescriptor *sd, int pointer_id, int value ){
        auto var_id = sd->getPointerVarType( pointer_id );
        string var_type = sd->getVarName( var_id );
        auto pointer_idx = sd->getPointerTypedIDX( pointer_id );
        setPointer(sd, var_type, pointer_idx, value );
    }

    void setPointer( StateDescriptor *sd, const string &pointer_name, int value ) {
        auto pointer_id = sd->getTypeID( pointer_name );
        setPointer( sd, pointer_id, value );
    }

    int getPointer( StateDescriptor *sd, const string &var_type, int idx ) const {
        auto var_idx = sd->getVarIDX( var_type );
        assert( var_idx < (int)_typed_pointers.size() );
        assert( idx < (int)_typed_pointers[ var_idx ].size() );
        return _typed_pointers[ var_idx ][ idx ];
    }

    int getPointer( StateDescriptor *sd, int pointer_id ) const {
        auto var_id = sd->getPointerVarType( pointer_id );
        string var_type = sd->getVarName( var_id );
        auto pointer_idx = sd->getPointerTypedIDX( pointer_id );
        return getPointer( sd, var_type, pointer_idx );
    }

    int getPointer( StateDescriptor *sd, const string &pointer_name ) const {
        auto pointer_id = sd->getTypeID( pointer_name );
        return getPointer( sd, pointer_id );
    }

    // In "var_obj" is encoded the object ids
    void addRegister( StateDescriptor *sd, const string &pred_type, const vector<int> &var_obj_idx, int value = 1 ){
        auto var_id_list = sd->getPredicateVarTypeIDs( pred_type );
        auto pred_idx = sd->getPredicateIDX( pred_type );
        assert( pred_idx < (int)_typed_registers.back().size() );
        _typed_registers.back()[ pred_idx ][ var_obj_idx ] = value;
    }

    // In "value" is encoded the var-object binding
    /*void delRegister( StateDescriptor *sd, const string &pred_type, const vector<int> &value ){
        auto pred_idx = sd->getPredicateIDX( pred_type );
        assert( pred_idx < (int)_typed_registers.size() );
        auto it = _typed_registers[ pred_idx ].find( value );
        if( it != _typed_registers[ pred_idx ].end() )
            _typed_registers[ pred_idx ].erase( it );
    }*/


    int getRegister( StateDescriptor *sd, 
    const string &pred_type, 
    const vector<int> &var_obj_idx, 
    const predicates& state_register) const{
        auto pred_idx = sd->getPredicateIDX( pred_type );
        assert( pred_idx < (int)state_register.size() );
        auto it = state_register[ pred_idx ].find( var_obj_idx );
        if( it == state_register[ pred_idx ].end() )
            return 0;
        return (it->second);
    }

    int getRegister( StateDescriptor *sd, 
    const string &pred_type, const vector<int> &var_obj_idx) const{
        return getRegister(sd, pred_type, var_obj_idx, _typed_registers.back());
    }

    void addEpistemicPredicate( const epistemic::predicate &epistemic_type){
        _typed_epistemic.emplace_back( epistemic_type );
        _epistemic_string_to_id[ epistemic_type.getName() ] = _typed_epistemic.size() - 1;
        _epistemic_history.insert( { epistemic_type, {} } );
    }

    void generateEpistemicHistory( const epistemic::predicate &epistemic_type) {
        assert(_epistemic_string_to_id.find(epistemic_type.getName()) != _epistemic_string_to_id.end());
        auto history_it = _epistemic_history.find(_typed_epistemic[_epistemic_string_to_id[epistemic_type.getName()]]);
        assert(history_it != _epistemic_history.end());
        
        
        epistemic::predicate& depend_predicate = _typed_epistemic[_epistemic_string_to_id[epistemic_type.getDependPredicate()]];
        history_it->second.emplace_back(
            EpistemicModel::updateLatestHistory(
                epistemic_type, getEpistemicHistory(depend_predicate, _lastest_history_index)
            )
        );
    }

    epistemic::history& getEpistemicHistory(const epistemic::predicate &epistemic_type, size_t history_length) {
        assert(_epistemic_history.find(epistemic_type) != _epistemic_history.end());
        if (_epistemic_history[epistemic_type].size() < history_length) {
            generateEpistemicHistory(epistemic_type);
        }
        return _epistemic_history[epistemic_type];
            
    }

    epistemic::history& getEpistemicHistory(const string &epistemic_name, size_t history_length) {
        return getEpistemicHistory(_typed_epistemic[_epistemic_string_to_id[epistemic_name]], history_length);
    }

    const epistemic::history& getConstEpistemicHistory(const string &epistemic_name) const {
        int epistemic_index = _epistemic_string_to_id.find(epistemic_name)->second;
        return _epistemic_history.find(_typed_epistemic[epistemic_index])->second;
    }

    void newGlobalState() {
        _typed_registers.push_back(_typed_registers.back());
    }

    void updateEpistemicHistory() {
        if (_typed_epistemic.empty()) {
            return;
        }
        _lastest_history_index++;
        for (const auto& epistemic_type : _typed_epistemic) {
            getEpistemicHistory(epistemic_type, _lastest_history_index);
        }
    }

    int getLatestEpistemicHistory( StateDescriptor *sd, Variable* v ) const {

        EpistemicVariable *epi = dynamic_cast<EpistemicVariable*>(v);
        assert( epi != nullptr );
        const epistemic::history& local_history = getConstEpistemicHistory(epi->getEpistemicPredicateName());
        Variable *lhs = epi->getLHS();
        Variable *rhs = epi->getRHS();
        // int id = lhs->getID();

        vector< int > params = lhs->getParameterIDs();
        vector< int > param_obj_idx ( params.size() );
        for( unsigned i = 0; i < params.size(); i++ ){
            assert( params[i] < 0 ); // the param is a constant object
            param_obj_idx[i] = -(params[i]+1); // constants are 0-indexed
        }

        int lhs_val = getRegister(sd, 
            sd->getPredicateName( lhs->getID() ), 
            param_obj_idx, 
            local_history.back()
        );

        assert( rhs->getVType() == VariableType::CONSTANT );
        int rhs_val = rhs->getID();

        return lhs_val == rhs_val;
    }
	
	vector< vector< int > > getStateVars() const{
		vector< vector< int > > state_vars = _typed_pointers;
		for( const auto& pred_regs : _typed_registers.back() ) {
		    for( const auto& sreg : pred_regs) {
		        // add only strictly positive valued predicates
		        if( sreg.second == 0 )
		            continue;
		        state_vars.emplace_back( sreg.first );
		        state_vars[ int( state_vars.size() )-1 ].emplace_back( sreg.second );
            }
        }
		return std::move(state_vars);
	}
	
	int size() const{
	    int res = 0;
	    for( const auto& tp : _typed_pointers )
	        res += int(tp.size());
	    for( const auto& tr : _typed_registers )
	        res += int(tr.size());
	    return res;
	}

	string toString() const{
		string ret = "[STATE]:";
		for( unsigned i = 0; i< _typed_pointers.size(); i++ ) {
            ret += "\nPOINTERS #" + to_string(i) +":";
            for( const auto p : _typed_pointers[i] ){
                ret += " " + to_string(p);
            }
        }
		for( unsigned i = 0; i < _typed_registers.back().size(); i++ ){
            ret += "\nREGISTERS #" + to_string(i) + ":";
            for( const auto& sv : _typed_registers.back()[ i ] ){
                ret += " (";
                for( int k = 0; k < int( sv.first.size()); k++ ){
                    ret += (k?",":"") + to_string( sv.first[k] );
                }
                ret += ")=" + to_string( sv.second );
            }
		}
		ret += "\n";
		return ret;
	}

	string toString( StateDescriptor *sd ){//, map< int, int > & var_id_to_num_objects ){
		string ret = "[STATE]:\n";

		vector< string > pointer_names = sd->getPointerNames();
		vector< string > var_types = sd->getVarTypes();
		vector< string > pred_types = sd->getPredicateTypes();

		for( unsigned i = 0; i < var_types.size(); i++ ){
            vector< int > pointer_ids = sd->getTypedPointerIDs( var_types[i] );
            for( unsigned j = 0; j < pointer_ids.size(); j++ ){
                int p_idx = sd->getPointerIDX( pointer_ids[j] );
                ret += "( " + var_types[i] + " " + pointer_names[ p_idx ] + ": " + to_string(_typed_pointers[i][j]) + " )\n";
            }
		}

		for( unsigned i = 0; i < pred_types.size(); i++ ){
		    ret += "+ " + pred_types[i] + "(";
		    vector< string > v_names = sd->getPredicateVarNames( pred_types[ i ] );
		    for( unsigned j = 0; j < v_names.size(); j++ ){
		        ret += (j?",":"") + v_names[j];
		    }
		    ret += "):";
		    for( const auto& sv : _typed_registers.back()[ i ] ){
		        ret += " (";
		        for( int k = 0; k < int( sv.first.size()); k++ ){
		            ret += (k?",":"") + to_string( sv.first[k] );
		        }
                ret += ")="+ to_string(sv.second);
		    }
		    ret += "\n";
		}
		return ret;
	}

	// ToDo test the next two functions
	void setInstanceID( int ins_id ){
        _instance_id = ins_id;
    }

	int getInstanceID() const{
        return _instance_id;
    }

    State::predicates& getLatestTypedRegisters() {
        return getEpistemicHistory("",_lastest_history_index).back();
    }


private:
    vector< vector< int > > _typed_pointers; // VarType -> {pointer1 = value1; ...; pointerN = valueN }
	/*
        key will be like "O [a] phi", "O [a] F [b] phi", "O [a] F [b] phi"
    */

    vector< epistemic::predicate > _typed_epistemic;
    unordered_map< string, int > _epistemic_string_to_id;
    std::unordered_map < epistemic::predicate, epistemic::history> _epistemic_history; // Epistemic history ( size = len(history) x required_history_type)
	epistemic::history& _typed_registers; // PredType ( size = |Obj1| x ... x |ObjM|;  or size=1 for 0-ary)
    size_t _lastest_history_index = 0;
	int _instance_id;
};

#endif
