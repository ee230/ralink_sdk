/*****************************************************
 ecmh - Easy Cast du Multi Hub - Common Functions
******************************************************
 $Author: chhung $
 $Id: common.h,v 1.1 2011-03-15 14:23:23 chhung Exp $
 $Date: 2011-03-15 14:23:23 $
*****************************************************/

void dolog(int level, const char *fmt, ...);
int huprunning(void);
void savepid(void);
void cleanpid(int i);
