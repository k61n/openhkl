#include <cmath>

namespace SX
{

namespace Utils
{

void doubleToFraction(double number, int nmax, long& nom, long& dnom)
{
    long m[2][2];
    double x;
    long maxden;
    long ai;

    x = number;
    maxden = nmax;

    /* initialize matrix */
    m[0][0] = m[1][1] = 1;
    m[0][1] = m[1][0] = 0;

    /* loop finding terms until denom gets too big */
    while (m[1][0] *  ( ai = (long)x ) + m[1][1] <= maxden) {
	long t;
	t = m[0][0] * ai + m[0][1];
	m[0][1] = m[0][0];
	m[0][0] = t;
	t = m[1][0] * ai + m[1][1];
	m[1][1] = m[1][0];
	m[1][0] = t;
        if(x==(double)ai) break;     // AF: division by zero
	x = 1/(x - (double) ai);
        if(x>(double)0x7FFFFFFF) break;  // AF: representation failure
    }

    /* now remaining x is between 0 and 1/ai */
    /* approx as either 0 or 1/m where m is max that will fit in maxden */
    /* first try zero */
    double n1=m[0][0],d1=m[1][0];

    /* now try other possibility */
    ai = (maxden - m[1][1]) / m[1][0];
    m[0][0] = m[0][0] * ai + m[0][1];
    m[1][0] = m[1][0] * ai + m[1][1];
    double n2=m[0][0],d2=m[1][0];

    if (d1<d2)
    {
    	nom=n1;
    	dnom=d1;
    }
    else
    {
    	nom=n2;
    	dnom=d2;
    }

    if (dnom<0)
    {
    	nom*=-1.0;
    	dnom*=-1.0;
    }

    return;
}

}

}

