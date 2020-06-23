/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef INIT_H
#define	INIT_H

#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
    
    void init_clock(void);
    void oscillatorInit(void);
    


#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* INIT_H */

