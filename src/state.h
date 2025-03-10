#ifndef __STATE_H__
#define __STATE_H__

#include "common.h"
#include "state_descriptor.h"

class State{
public:
    // Default constructor
    State(){
        _instance_id = 0;
    }

    // Constructor mainly used for initial states
	explicit State( StateDescriptor *sd, int instance_id = 0 ){
        auto var_types = sd->getVarTypes();
        _typed_pointers.resize( var_types.size() );
        for( int i = 0; i < int( var_types.size() ); i++ ){
            auto num_typed_pointers = sd->getNumTypedPointers( var_types[i] );
            _typed_pointers[i].resize( num_typed_pointers, 0 );
        }

        auto pred_types = sd->getPredicateTypes();
        _typed_registers.resize( pred_types.size() );
        _instance_id = instance_id;
	}

	// Constructor mainly used to make copies and propagate effects
	explicit State( State *s ){
		_typed_pointers = s->getTypedPointers();
		_typed_registers = s->getTypedRegisters();
		_instance_id = s->getInstanceID();
	}
	
	State* copy() {
		return new State( this );
	}

	const vector< vector< int > >& getTypedPointers() const{
	    return _typed_pointers;
	}

	const vector< map< vector<int>, int > >& getTypedRegisters() const{
	    return _typed_registers;
	}

	void setPointersSize( unsigned p_size ){
        _typed_pointers.resize( p_size );
    }

    void setRegistersSize( unsigned r_size ){
        _typed_registers.resize( r_size );
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
        assert( pred_idx < (int)_typed_registers.size() );
        _typed_registers[ pred_idx ][ var_obj_idx ] = value;
    }

    // In "value" is encoded the var-object binding
    /*void delRegister( StateDescriptor *sd, const string &pred_type, const vector<int> &value ){
        auto pred_idx = sd->getPredicateIDX( pred_type );
        assert( pred_idx < (int)_typed_registers.size() );
        auto it = _typed_registers[ pred_idx ].find( value );
        if( it != _typed_registers[ pred_idx ].end() )
            _typed_registers[ pred_idx ].erase( it );
    }*/

    int getRegister( StateDescriptor *sd, const string &pred_type, const vector<int> &var_obj_idx ) const{
        auto pred_idx = sd->getPredicateIDX( pred_type );
        assert( pred_idx < (int)_typed_registers.size() );
        auto it = _typed_registers[ pred_idx ].find( var_obj_idx );
        if( it == _typed_registers[ pred_idx ].end() )
            return 0;
        return (it->second);
    }
	
	vector< vector< int > > getStateVars() const{
		vector< vector< int > > state_vars = _typed_pointers;
		for( const auto& pred_regs : _typed_registers ) {
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
		for( unsigned i = 0; i < _typed_registers.size(); i++ ){
            ret += "\nREGISTERS #" + to_string(i) + ":";
            for( const auto& sv : _typed_registers[ i ] ){
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
		    for( const auto& sv : _typed_registers[ i ] ){
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

private:
    vector< vector< int > > _typed_pointers; // VarType -> {pointer1 = value1; ...; pointerN = valueN }
	vector< map< vector< int >, int > > _typed_registers; // PredType ( size = |Obj1| x ... x |ObjM|;  or size=1 for 0-ary)
	int _instance_id;
};

#endif
