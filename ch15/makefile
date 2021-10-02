OBJS = get_header_list.o add_header_dimension.o add_tuple.o \
       del_header_element.o del_node.o del_tuple.o get_node_list.o \
       get_range_max.o get_range_min.o hdr_list_element_get.o \
       ins_header_element.o ins_node.o next_node.o previous_node.o \
       retrieve_node.o tuple_dim.o

testme : $(OBJS) testme.o
	gcc -ansi $(OBJS) testme.o -o testme

testme.o : testme.c sparse.h
	gcc -ansi -c testme.c

get_header_list.o : get_header_list.c sparse.h
	gcc -ansi -c get_header_list.c 

add_header_dimension.o : add_header_dimension.c sparse.h
	gcc -ansi -c add_header_dimension.c 

add_tuple.o : add_tuple.c sparse.h
	gcc -ansi -c add_tuple.c 

del_header_element.o : del_header_element.c sparse.h
	gcc -ansi -c del_header_element.c 

del_node.o : del_node.c sparse.h
	gcc -ansi -c del_node.c 

del_tuple.o : del_tuple.c sparse.h
	gcc -ansi -c del_tuple.c 

get_node_list.o : get_node_list.c sparse.h
	gcc -ansi -c get_node_list.c 

get_range_max.o : get_range_max.c sparse.h
	gcc -ansi -c get_range_max.c 

get_range_min.o : get_range_min.c sparse.h
	gcc -ansi -c get_range_min.c 

hdr_list_element_get.o : hdr_list_element_get.c sparse.h
	gcc -ansi -c hdr_list_element_get.c 

ins_header_element.o : ins_header_element.c sparse.h
	gcc -ansi -c ins_header_element.c 

ins_node.o : ins_node.c sparse.h
	gcc -ansi -c ins_node.c 

next_node.o : next_node.c sparse.h
	gcc -ansi -c next_node.c 

previous_node.o : previous_node.c sparse.h
	gcc -ansi -c previous_node.c 

retrieve_node.o : retrieve_node.c sparse.h
	gcc -ansi -c retrieve_node.c 

tuple_dim.o : tuple_dim.c sparse.h
	gcc -ansi -c tuple_dim.c 
