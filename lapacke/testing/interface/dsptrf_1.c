/*****************************************************************************
  Copyright (c) 2010, Intel Corp.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/
/*  Contents: test routine for C interface to LAPACK
*   Author: Intel Corporation
*   Created in March, 2010
*
* Purpose
*
* dsptrf_1 is the test program for the C interface to LAPACK
* routine dsptrf
* The program doesn't require an input, the input data is hardcoded in the
* test program.
* The program tests the C interface in the four combinations:
*   1) column-major layout, middle-level interface
*   2) column-major layout, high-level interface
*   3) row-major layout, middle-level interface
*   4) row-major layout, high-level interface
* The output of the C interface function is compared to those obtained from
* the corresponiding LAPACK routine with the same input data, and the
* comparison diagnostics is then printed on the standard output having PASSED
* keyword if the test is passed, and FAILED keyword if the test isn't passed.
*****************************************************************************/
#include <stdio.h>
#include "lapacke.h"
#include "lapacke_utils.h"
#include "test_utils.h"

static void init_scalars_dsptrf( char *uplo, lapack_int *n );
static void init_ap( lapack_int size, double *ap );
static void init_ipiv( lapack_int size, lapack_int *ipiv );
static int compare_dsptrf( double *ap, double *ap_i, lapack_int *ipiv,
                           lapack_int *ipiv_i, lapack_int info,
                           lapack_int info_i, lapack_int n );

int main(void)
{
    /* Local scalars */
    char uplo, uplo_i;
    lapack_int n, n_i;
    lapack_int info, info_i;
    lapack_int i;
    int failed;

    /* Local arrays */
    double *ap = NULL, *ap_i = NULL;
    lapack_int *ipiv = NULL, *ipiv_i = NULL;
    double *ap_save = NULL;
    lapack_int *ipiv_save = NULL;
    double *ap_r = NULL;

    /* Iniitialize the scalar parameters */
    init_scalars_dsptrf( &uplo, &n );
    uplo_i = uplo;
    n_i = n;

    /* Allocate memory for the LAPACK routine arrays */
    ap = (double *)LAPACKE_malloc( ((n*(n+1)/2)) * sizeof(double) );
    ipiv = (lapack_int *)LAPACKE_malloc( n * sizeof(lapack_int) );

    /* Allocate memory for the C interface function arrays */
    ap_i = (double *)LAPACKE_malloc( ((n*(n+1)/2)) * sizeof(double) );
    ipiv_i = (lapack_int *)LAPACKE_malloc( n * sizeof(lapack_int) );

    /* Allocate memory for the backup arrays */
    ap_save = (double *)LAPACKE_malloc( ((n*(n+1)/2)) * sizeof(double) );
    ipiv_save = (lapack_int *)LAPACKE_malloc( n * sizeof(lapack_int) );

    /* Allocate memory for the row-major arrays */
    ap_r = (double *)LAPACKE_malloc( n*(n+1)/2 * sizeof(double) );

    /* Initialize input arrays */
    init_ap( (n*(n+1)/2), ap );
    init_ipiv( n, ipiv );

    /* Backup the ouptut arrays */
    for( i = 0; i < (n*(n+1)/2); i++ ) {
        ap_save[i] = ap[i];
    }
    for( i = 0; i < n; i++ ) {
        ipiv_save[i] = ipiv[i];
    }

    /* Call the LAPACK routine */
    dsptrf_( &uplo, &n, ap, ipiv, &info );

    /* Initialize input data, call the column-major middle-level
     * interface to LAPACK routine and check the results */
    for( i = 0; i < (n*(n+1)/2); i++ ) {
        ap_i[i] = ap_save[i];
    }
    for( i = 0; i < n; i++ ) {
        ipiv_i[i] = ipiv_save[i];
    }
    info_i = LAPACKE_dsptrf_work( LAPACK_COL_MAJOR, uplo_i, n_i, ap_i, ipiv_i );

    failed = compare_dsptrf( ap, ap_i, ipiv, ipiv_i, info, info_i, n );
    if( failed == 0 ) {
        printf( "PASSED: column-major middle-level interface to dsptrf\n" );
    } else {
        printf( "FAILED: column-major middle-level interface to dsptrf\n" );
    }

    /* Initialize input data, call the column-major high-level
     * interface to LAPACK routine and check the results */
    for( i = 0; i < (n*(n+1)/2); i++ ) {
        ap_i[i] = ap_save[i];
    }
    for( i = 0; i < n; i++ ) {
        ipiv_i[i] = ipiv_save[i];
    }
    info_i = LAPACKE_dsptrf( LAPACK_COL_MAJOR, uplo_i, n_i, ap_i, ipiv_i );

    failed = compare_dsptrf( ap, ap_i, ipiv, ipiv_i, info, info_i, n );
    if( failed == 0 ) {
        printf( "PASSED: column-major high-level interface to dsptrf\n" );
    } else {
        printf( "FAILED: column-major high-level interface to dsptrf\n" );
    }

    /* Initialize input data, call the row-major middle-level
     * interface to LAPACK routine and check the results */
    for( i = 0; i < (n*(n+1)/2); i++ ) {
        ap_i[i] = ap_save[i];
    }
    for( i = 0; i < n; i++ ) {
        ipiv_i[i] = ipiv_save[i];
    }

    LAPACKE_dpp_trans( LAPACK_COL_MAJOR, uplo, n, ap_i, ap_r );
    info_i = LAPACKE_dsptrf_work( LAPACK_ROW_MAJOR, uplo_i, n_i, ap_r, ipiv_i );

    LAPACKE_dpp_trans( LAPACK_ROW_MAJOR, uplo, n, ap_r, ap_i );

    failed = compare_dsptrf( ap, ap_i, ipiv, ipiv_i, info, info_i, n );
    if( failed == 0 ) {
        printf( "PASSED: row-major middle-level interface to dsptrf\n" );
    } else {
        printf( "FAILED: row-major middle-level interface to dsptrf\n" );
    }

    /* Initialize input data, call the row-major high-level
     * interface to LAPACK routine and check the results */
    for( i = 0; i < (n*(n+1)/2); i++ ) {
        ap_i[i] = ap_save[i];
    }
    for( i = 0; i < n; i++ ) {
        ipiv_i[i] = ipiv_save[i];
    }

    /* Init row_major arrays */
    LAPACKE_dpp_trans( LAPACK_COL_MAJOR, uplo, n, ap_i, ap_r );
    info_i = LAPACKE_dsptrf( LAPACK_ROW_MAJOR, uplo_i, n_i, ap_r, ipiv_i );

    LAPACKE_dpp_trans( LAPACK_ROW_MAJOR, uplo, n, ap_r, ap_i );

    failed = compare_dsptrf( ap, ap_i, ipiv, ipiv_i, info, info_i, n );
    if( failed == 0 ) {
        printf( "PASSED: row-major high-level interface to dsptrf\n" );
    } else {
        printf( "FAILED: row-major high-level interface to dsptrf\n" );
    }

    /* Release memory */
    if( ap != NULL ) {
        LAPACKE_free( ap );
    }
    if( ap_i != NULL ) {
        LAPACKE_free( ap_i );
    }
    if( ap_r != NULL ) {
        LAPACKE_free( ap_r );
    }
    if( ap_save != NULL ) {
        LAPACKE_free( ap_save );
    }
    if( ipiv != NULL ) {
        LAPACKE_free( ipiv );
    }
    if( ipiv_i != NULL ) {
        LAPACKE_free( ipiv_i );
    }
    if( ipiv_save != NULL ) {
        LAPACKE_free( ipiv_save );
    }

    return 0;
}

/* Auxiliary function: dsptrf scalar parameters initialization */
static void init_scalars_dsptrf( char *uplo, lapack_int *n )
{
    *uplo = 'L';
    *n = 4;

    return;
}

/* Auxiliary functions: dsptrf array parameters initialization */
static void init_ap( lapack_int size, double *ap ) {
    lapack_int i;
    for( i = 0; i < size; i++ ) {
        ap[i] = 0;
    }
    ap[0] = 2.06999999999999980e+000;
    ap[1] = 3.87000000000000010e+000;
    ap[2] = 4.20000000000000020e+000;
    ap[3] = -1.14999999999999990e+000;
    ap[4] = -2.09999999999999990e-001;
    ap[5] = 1.87000000000000010e+000;
    ap[6] = 6.30000000000000000e-001;
    ap[7] = 1.14999999999999990e+000;
    ap[8] = 2.06000000000000010e+000;
    ap[9] = -1.81000000000000010e+000;
}
static void init_ipiv( lapack_int size, lapack_int *ipiv ) {
    lapack_int i;
    for( i = 0; i < size; i++ ) {
        ipiv[i] = 0;
    }
}

/* Auxiliary function: C interface to dsptrf results check */
/* Return value: 0 - test is passed, non-zero - test is failed */
static int compare_dsptrf( double *ap, double *ap_i, lapack_int *ipiv,
                           lapack_int *ipiv_i, lapack_int info,
                           lapack_int info_i, lapack_int n )
{
    lapack_int i;
    int failed = 0;
    for( i = 0; i < (n*(n+1)/2); i++ ) {
        failed += compare_doubles(ap[i],ap_i[i]);
    }
    for( i = 0; i < n; i++ ) {
        failed += (ipiv[i] == ipiv_i[i]) ? 0 : 1;
    }
    failed += (info == info_i) ? 0 : 1;
    if( info != 0 || info_i != 0 ) {
        printf( "info=%d, info_i=%d\n",(int)info,(int)info_i );
    }

    return failed;
}
