/*---------------------------------------------------------------
 * Copyright (c) 1999,2000,2001,2002,2003
 * The Board of Trustees of the University of Illinois
 * All Rights Reserved.
 *---------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software (Iperf) and associated
 * documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 *
 * Redistributions of source code must retain the above
 * copyright notice, this list of conditions and
 * the following disclaimers.
 *
 *
 * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimers in the documentation and/or other materials
 * provided with the distribution.
 *
 *
 * Neither the names of the University of Illinois, NCSA,
 * nor the names of its contributors may be used to endorse
 * or promote products derived from this Software without
 * specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE CONTIBUTORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * ________________________________________________________________
 * National Laboratory for Applied Network Research
 * National Center for Supercomputing Applications
 * University of Illinois at Urbana-Champaign
 * http://www.ncsa.uiuc.edu
 * ________________________________________________________________
 *
 * stdio.c
 * by Mark Gates <mgates@nlanr.net>
 * and Ajay Tirumalla <tirumala@ncsa.uiuc.edu>
 * -------------------------------------------------------------------
 * input and output numbers, converting with kilo, mega, giga
 * ------------------------------------------------------------------- */

 #include <stdio.h>
 #include <assert.h>
 #include <ctype.h>
 #include <string.h>
 #ifdef HAVE_STDINT_H
 #include <stdint.h>
 #endif
 #include <sys/socket.h>
 #include <sys/time.h>
 #include <netinet/tcp.h>

 #include "iperf.h"
 #ifdef __cplusplus
 {
     #endif

     const long KILO_UNIT = 1024;
     const long MEGA_UNIT = 1024 * 1024;
     const long GIGA_UNIT = 1024 * 1024 * 1024;

     const long KILO_RATE_UNIT = 1000;
     const long MEGA_RATE_UNIT = 1000 * 1000;
     const long GIGA_RATE_UNIT = 1000 * 1000 * 1000;

     /* -------------------------------------------------------------------
     * unit_atof
     *
     * Given a string of form #x where # is a number and x is a format
     * character listed below, this returns the interpreted integer.
     * Gg, Mm, Kk are giga, mega, kilo respectively
     * ------------------------------------------------------------------- */

     double    unit_atof(const char *s)
     {
         double    n;
         char      suffix = '\0';

         assert(s != NULL);

         /* scan the number and any suffices */
         sscanf(s, "%lf%c", &n, &suffix);

         /* convert according to [Gg Mm Kk] */
         switch    (suffix)
         {
             case 'g': case 'G':
             n *= GIGA_UNIT;
             break;
             case 'm': case 'M':
             n *= MEGA_UNIT;
             break;
             case 'k': case 'K':
             n *= KILO_UNIT;
             break;
             default:
             break;
         }
         return n;
     }				/* end unit_atof */


     /* -------------------------------------------------------------------
     * unit_atof_rate
     *
     * Similar to unit_atof, but uses 10-based rather than 2-based
     * suffixes.
     * ------------------------------------------------------------------- */

     double    unit_atof_rate(const char *s)
     {
         double    n;
         char      suffix = '\0';

         assert(s != NULL);

         /* scan the number and any suffices */
         sscanf(s, "%lf%c", &n, &suffix);

         /* convert according to [Gg Mm Kk] */
         switch    (suffix)
         {
             case 'g': case 'G':
             n *= GIGA_RATE_UNIT;
             break;
             case 'm': case 'M':
             n *= MEGA_RATE_UNIT;
             break;
             case 'k': case 'K':
             n *= KILO_RATE_UNIT;
             break;
             default:
             break;
         }
         return n;
     }				/* end unit_atof_rate */



     /* -------------------------------------------------------------------
     * unit_atoi
     *
     * Given a string of form #x where # is a number and x is a format
     * character listed below, this returns the interpreted integer.
     * Gg, Mm, Kk are giga, mega, kilo respectively
     * ------------------------------------------------------------------- */

     iperf_size_t unit_atoi(const char *s)
     {
         double    n;
         char      suffix = '\0';

         assert(s != NULL);

         /* scan the number and any suffices */
         sscanf(s, "%lf%c", &n, &suffix);

         /* convert according to [Gg Mm Kk] */
         switch    (suffix)
         {
             case 'g': case 'G':
             n *= GIGA_UNIT;
             break;
             case 'm': case 'M':
             n *= MEGA_UNIT;
             break;
             case 'k': case 'K':
             n *= KILO_UNIT;
             break;
             default:
             break;
         }
         return (iperf_size_t) n;
     }				/* end unit_atof */

     /* -------------------------------------------------------------------
     * constants for byte_printf
     * ------------------------------------------------------------------- */

     /* used as indices into conversion_bytes[], label_byte[], and label_bit[] */
     enum
     {
         UNIT_CONV,
         KILO_CONV,
         MEGA_CONV,
         GIGA_CONV
     };

     /* factor to multiply the number by */
     const double conversion_bytes[] =
     {
         1.0,			/* unit */
         1.0 / 1024,		/* kilo */
         1.0 / 1024 / 1024,	/* mega */
         1.0 / 1024 / 1024 / 1024/* giga */
     };

     /* factor to multiply the number by for bits*/
     const double conversion_bits[] =
     {
         1.0,			/* unit */
         1.0 / 1000,		/* kilo */
         1.0 / 1000 / 1000,	/* mega */
         1.0 / 1000 / 1000 / 1000/* giga */
     };


     /* labels for Byte formats [KMG] */
     const char *label_byte[] =
     {
         "Byte",
         "KByte",
         "MByte",
         "GByte"
     };

     /* labels for bit formats [kmg] */
     const char *label_bit[] =
     {
         "bit",
         "Kbit",
         "Mbit",
         "Gbit"
     };

     /* -------------------------------------------------------------------
     * unit_snprintf
     *
     * Given a number in bytes and a format, converts the number and
     * prints it out with a bits or bytes label.
     *   B, K, M, G, A for Byte, Kbyte, Mbyte, Gbyte, adaptive byte
     *   b, k, m, g, a for bit,  Kbit,  Mbit,  Gbit,  adaptive bit
     * adaptive picks the "best" one based on the number.
     * s should be at least 11 chars long
     * (4 digits + space + 5 chars max + null)
     * ------------------------------------------------------------------- */

     FILE *gnuplot;

     int curind = 0;
     int size = MAX_TIME;
     double x[MAX_TIME];
     double y[MAX_TIME];
     double empty[MAX_TIME];
     char currole;
     int visualstat = 1;
     double lowy= -100;
     double highy = 0;
     char titleclient[] = "Client";
     char titleserver[] = "Server";
     int duration = 10;
     char unit = 'G';
     float senttotal;
     double xvar = 0;
     double interval = 1;

     void      setcurrentrole(char* r){
         currole = r;
     }
     void      visualizerstatus(int stat){
         visualstat = stat;
     }
     void      setduration(int * t){
         duration = t;
     }
     int      getvisstatus(){
         return visualstat;
     }
     void      setunit(char* u){
         unit = u;
     }
     void      setinterval(double t){
         interval = t;
         xvar = interval;
     }
     void      printtotal(){
         float t;
         int i;
         for(i = 1; i<duration; i++){
             t += y[i];
         }
         t = t/duration;
         fprintf(stderr, "Total:  0.00-%3.2f sec  %3.1f %cBytes  %4.1f %cbits/sec\n", (float)duration,senttotal, unit, t, unit);
     }

     void      resetgnuplot(){
         if(gnuplot != NULL){
             fprintf(gnuplot, "pause mouse close\n");
             fclose(gnuplot);
             gnuplot = NULL;
             curind = 0;
             xvar = 0;
             memcpy(x,empty,sizeof(x));
             memcpy(y,empty,sizeof(y));
             lowy = -100;
             highy = 0;
         }

     }
     void      setcurrentip(char* ip){
         if (gnuplot == NULL){
             gnuplot = popen("gnuplot", "w");
             if(currole == 'c'){
                 fprintf(gnuplot, "set title \"%s - %s\"\n", titleclient, ip);
             }
         }
     }




     void      unit_snprintf(char *s, int inLen, double inNum, char inFormat){
         int conv;
         int i;
         const char *suffix;
         const char *format;
         /* convert to bits for [bkmga] */
         if(!isupper((int) inFormat)){
             inNum *= 8;
         }
         switch    (toupper(inFormat))
         {
             case 'B':
             conv = UNIT_CONV;
             break;
             case 'K':
             conv = KILO_CONV;
             break;
             case 'M':
             conv = MEGA_CONV;
             break;
             case 'G':
             conv = GIGA_CONV;
             break;

             default:
             case 'A':
             {
                 double    tmpNum = inNum;
                 conv = UNIT_CONV;

                 if (isupper((int) inFormat))
                 {
                     while (tmpNum >= 1024.0 && conv <= GIGA_CONV)
                     {
                         tmpNum /= 1024.0;
                         conv++;
                     }
                 } else
                 {
                     while (tmpNum >= 1000.0 && conv <= GIGA_CONV)
                     {
                         tmpNum /= 1000.0;
                         conv++;
                     }
                 }
                 break;
             }
         }

         if (!isupper((int) inFormat))
         {
             inNum *= conversion_bits[conv];
             suffix = label_bit[conv];
         } else
         {
             inNum *= conversion_bytes[conv];
             suffix = label_byte[conv];
         }

         /* print such that we always fit in 4 places */
         if (inNum < 9.995)
         {			/* 9.995 would be rounded to 10.0 */
             format = "%4.2f %s";/* #.## */
         } else if (inNum < 99.95)
         {			/* 99.95 would be rounded to 100 */
             format = "%4.1f %s";/* ##.# */
         } else if (inNum < 999.5)
         {			/* 999.5 would be rounded to 1000 */
             format = "%4.0f %s";/* ### */
         } else
         {			/* 1000-1024 fits in 4 places If not using
             * Adaptive sizes then this code will not
             * control spaces */
             format = "%4.0f %s";/* #### */
         }
         if (strcmp(suffix, "GByte") == 0){
             if(unit == 'G' || unit == 'g'){
                 senttotal += inNum;
             }
             else if (unit == 'M' || unit == 'm'){
                 senttotal += (1000 * inNum);
             }
         }
         if (strcmp(suffix, "MByte") == 0){
             if(unit == 'G' || unit == 'g'){
                 senttotal += (inNum/1000);
             }
             else if (unit == 'M' || unit == 'm'){
                 senttotal += inNum;
             }
         }

         if (visualstat == 1){
             if (gnuplot == NULL){
                 gnuplot = popen("gnuplot", "w");
                 if(currole == 's'){
                     fprintf(gnuplot, "set title \"Server\"\n");
                 }
             }

             if (strcmp(suffix, "Gbit") == 0){
                 //data for yrange

                 double converted;
                 int spacing;
                 if (unit == 'g' || unit == 'G'){
                     fprintf(gnuplot, "set ylabel \"Gbits/Sec\"\n");
                     converted = inNum;
                     spacing = 2;
                 }
                 else if (unit == 'm' || unit == 'M'){
                     fprintf(gnuplot, "set ylabel \"Mbits/Sec\"\n");
                     converted = inNum * 1000;
                     spacing = 2000;
                 }
                 else if (unit == 'k' || unit == 'K'){
                     fprintf(gnuplot, "set ylabel \"Kbits/Sec\"\n");
                     converted = inNum * 1000000;
                     spacing = 1000000;
                 }
                 if(highy < converted){highy = converted;}
                 if(lowy == -100 || lowy > converted){lowy = converted;}
                 //setting data
                 y[curind] = converted;
                 x[curind] = xvar;

                 xvar += interval;




                 //setting viewrange to limits
                 if(curind >= 2){		//color differentiation and plotting
                     fprintf(gnuplot, "set yrange [%f:%f]\n",lowy-spacing,highy+spacing);
                     if(xvar == duration-1){
                         fprintf(gnuplot, "set xrange [*:*]\n");
                     }
                     else if(xvar > 30 && currole != 's'){
                         fprintf(gnuplot, "set xrange [%g:%g]\n", xvar-30,xvar);
                     }


                     if(currole == 'c'){
                         fprintf(gnuplot, "plot '-' with linespoints lw 2 linecolor rgb \"#6aa194\" pt 7 notitle\n");
                     }
                     else if (currole == 's'){
                         fprintf(gnuplot, "plot '-' with linespoints lw 2 linecolor rgb \"#946aa1\" pt 7 notitle\n");
                     }


                     //inputting data
                     for (i = 0; i <= curind; i++){
                         fprintf(gnuplot, "%g %g\n", x[i], y[i]);
                    }


                     fprintf(gnuplot, "e\n");


                 }
                 curind += 1;
                 fflush(gnuplot);
             }
             if (strcmp(suffix, "Mbit") == 0){
                 //data for yrange

                 double converted;
                 int spacing;
                 if (unit == 'g' || unit == 'G'){
                     fprintf(gnuplot, "set ylabel \"Gbits/Sec\"\n");
                     converted = inNum*1000;
                     spacing = 2000;
                 }
                 else if (unit == 'm' || unit == 'M'){
                     fprintf(gnuplot, "set ylabel \"Mbits/Sec\"\n");
                     converted = inNum;
                     spacing = 2;
                 }
                 else if (unit == 'k' || unit == 'K'){
                     fprintf(gnuplot, "set ylabel \"Kbits/Sec\"\n");
                     converted = inNum * 1000;
                     spacing = 1000;
                 }
                 if(highy < converted){highy = converted;}
                 if(lowy == -100 || lowy > converted){lowy = converted;}
                 //setting data
                 y[curind] = converted;
                 x[curind] = xvar;

                 xvar += interval;




                 //setting viewrange to limits
                 if(curind >= 2){		//color differentiation and plotting
                     fprintf(gnuplot, "set yrange [%f:%f]\n",lowy-spacing,highy+spacing);
                     if(xvar == duration-1){
                         fprintf(gnuplot, "set xrange [*:*]\n");
                     }
                     else if(xvar > 30 && currole != 's'){
                         fprintf(gnuplot, "set xrange [%g:%g]\n", xvar-30,xvar);
                     }


                     if(currole == 'c'){
                         fprintf(gnuplot, "plot '-' with linespoints lw 2 linecolor rgb \"#6aa194\" pt 7 notitle\n");
                     }
                     else if (currole == 's'){
                         fprintf(gnuplot, "plot '-' with linespoints lw 2 linecolor rgb \"#946aa1\" pt 7 notitle\n");
                     }


                     //inputting data
                     for (i = 0; i <= curind; i++){
                         fprintf(gnuplot, "%g %g\n", x[i], y[i]);
                    }


                     fprintf(gnuplot, "e\n");


                 }
                 curind += 1;
                 fflush(gnuplot);
             }
         }









         snprintf(s, inLen, format, inNum, suffix);
         if(xvar == duration-1 && currole != 's'){
             fprintf(gnuplot, "pause mouse close\n");
         }
     }				/* end unit_snprintf */

     #ifdef __cplusplus
 }				/* end extern "C" */

 #endif
