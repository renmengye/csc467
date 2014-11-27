/*
 * reg_conserve.h
 *
 *  Created on: 2014-11-27
 *      Author: dot
 */

#ifndef REG_CONSERVE_H_
#define REG_CONSERVE_H_



typedef struct _reg_list{
	char *name;
	char *replaced_name;
	int is_free;

	struct _reg_list *next;
} reg_list;




#endif /* REG_CONSERVE_H_ */
