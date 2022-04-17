#include "ADTSet.h"

Pointer set_find_eq_or_greater(Set set, Pointer value){
    SetNode begin = SET_BOF;
    while(set_node_value(set, set_next(set, begin)) < value && set_next(set, begin) != SET_EOF){
        begin = set_next(set, begin);
    }
    if(set_node_value(set, begin) == value){
        return set_node_value(set, begin);
    }
    else if(set_next(set, begin) > value){
        return set_node_value(set, begin);
    }
    else{
        return NULL;
    }
}

Pointer set_find_eq_or_smaller(Set set, Pointer value){
    if(set_size(set) == 0){
        return NULL;
    }
    SetNode prev = SET_BOF;
    SetNode current = set_next(set, prev);
    while(current != SET_EOF){
        if(set_node_value(set, current) == value){
            return set_node_value(set, current);
        }
        else if(set_node_value(set,current) > value){
            if(prev != SET_BOF){
                return set_node_value(set, prev);
            }
            else{
                return NULL;
            }
        }
        prev = current;
        current = set_next(set, current);
    }
}


