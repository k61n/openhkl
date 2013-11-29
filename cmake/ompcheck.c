/*
 * Multi precision toolbox for Scilab
 * Copyright (C) 2009 - Jonathan Blanchard
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 */

/* Check if the compiler support OpenMP. */

#include <omp.h>

int main (int argc, char *argv[])
{
    int array[100];
    unsigned int i;

    omp_set_num_threads(2);


    #pragma omp parallel for schedule( dynamic )
    for (i = 0; i < 100; i++)
    {
        array[i] = i+i;
    }

    return 0;
}
