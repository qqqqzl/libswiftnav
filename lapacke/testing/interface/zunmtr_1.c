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
* zunmtr_1 is the test program for the C interface to LAPACK
* routine zunmtr
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

static void init_scalars_zunmtr( char *side, char *uplo, char *trans,
                                 lapack_int *m, lapack_int *n, lapack_int *lda,
                                 lapack_int *ldc, lapack_int *lwork );
static void init_a( lapack_int size, lapack_complex_double *a );
static void init_tau( lapack_int size, lapack_complex_double *tau );
static void init_c( lapack_int size, lapack_complex_double *c );
static void init_work( lapack_int size, lapack_complex_double *work );
static int compare_zunmtr( lapack_complex_double *c, lapack_complex_double *c_i,
                           lapack_int info, lapack_int info_i, lapack_int ldc,
                           lapack_int n );

int main(void)
{
    /* Local scalars */
    char side, side_i;
    char uplo, uplo_i;
    char trans, trans_i;
    lapack_int m, m_i;
    lapack_int n, n_i;
    lapack_int lda, lda_i;
    lapack_int lda_r;
    lapack_int ldc, ldc_i;
    lapack_int ldc_r;
    lapack_int lwork, lwork_i;
    lapack_int info, info_i;
    /* Declare scalars */
    lapack_int r;
    lapack_int i;
    int failed;

    /* Local arrays */
    lapack_complex_double *a = NULL, *a_i = NULL;
    lapack_complex_double *tau = NULL, *tau_i = NULL;
    lapack_complex_double *c = NULL, *c_i = NULL;
    lapack_complex_double *work = NULL, *work_i = NULL;
    lapack_complex_double *c_save = NULL;
    lapack_complex_double *a_r = NULL;
    lapack_complex_double *c_r = NULL;

    /* Iniitialize the scalar parameters */
    init_scalars_zunmtr( &side, &uplo, &trans, &m, &n, &lda, &ldc, &lwork );
    r = LAPACKE_lsame( side, 'l' ) ? m : n;
    lda_r = r+2;
    ldc_r = n+2;
    side_i = side;
    uplo_i = uplo;
    trans_i = trans;
    m_i = m;
    n_i = n;
    lda_i = lda;
    ldc_i = ldc;
    lwork_i = lwork;

    /* Allocate memory for the LAPACK routine arrays */
    a = (lapack_complex_double *)
        LAPACKE_malloc( lda*m * sizeof(lapack_complex_double) );
    tau = (lapack_complex_double *)
        LAPACKE_malloc( (m-1) * sizeof(lapack_complex_double) );
    c = (lapack_complex_double *)
        LAPACKE_malloc( ldc*n * sizeof(lapack_complex_double) );
    work = (lapack_complex_double *)
        LAPACKE_malloc( lwork * sizeof(lapack_complex_double) );

    /* Allocate memory for the C interface function arrays */
    a_i = (lapack_complex_double *)
        LAPACKE_malloc( lda*m * sizeof(lapack_complex_double) );
    tau_i = (lapack_complex_double *)
        LAPACKE_malloc( (m-1) * sizeof(lapack_complex_double) );
    c_i = (lapack_complex_double *)
        LAPACKE_malloc( ldc*n * sizeof(lapack_complex_double) );
    work_i = (lapack_complex_double *)
        LAPACKE_malloc( lwork * sizeof(lapack_complex_double) );

    /* Allocate memory for the backup arrays */
    c_save = (lapack_complex_double *)
        LAPACKE_malloc( ldc*n * sizeof(lapack_complex_double) );

    /* Allocate memory for the row-major arrays */
    a_r = (lapack_complex_double *)
        LAPACKE_malloc( r*(r+2) * sizeof(lapack_complex_double) );
    c_r = (lapack_complex_double *)
        LAPACKE_malloc( m*(n+2) * sizeof(lapack_complex_double) );

    /* Initialize input arrays */
    init_a( lda*m, a );
    init_tau( (m-1), tau );
    init_c( ldc*n, c );
    init_work( lwork, work );

    /* Backup the ouptut arrays */
    for( i = 0; i < ldc*n; i++ ) {
        c_save[i] = c[i];
    }

    /* Call the LAPACK routine */
    zunmtr_( &side, &uplo, &trans, &m, &n, a, &lda, tau, c, &ldc, work, &lwork,
             &info );

    /* Initialize input data, call the column-major middle-level
     * interface to LAPACK routine and check the results */
    for( i = 0; i < lda*m; i++ ) {
        a_i[i] = a[i];
    }
    for( i = 0; i < (m-1); i++ ) {
        tau_i[i] = tau[i];
    }
    for( i = 0; i < ldc*n; i++ ) {
        c_i[i] = c_save[i];
    }
    for( i = 0; i < lwork; i++ ) {
        work_i[i] = work[i];
    }
    info_i = LAPACKE_zunmtr_work( LAPACK_COL_MAJOR, side_i, uplo_i, trans_i,
                                  m_i, n_i, a_i, lda_i, tau_i, c_i, ldc_i,
                                  work_i, lwork_i );

    failed = compare_zunmtr( c, c_i, info, info_i, ldc, n );
    if( failed == 0 ) {
        printf( "PASSED: column-major middle-level interface to zunmtr\n" );
    } else {
        printf( "FAILED: column-major middle-level interface to zunmtr\n" );
    }

    /* Initialize input data, call the column-major high-level
     * interface to LAPACK routine and check the results */
    for( i = 0; i < lda*m; i++ ) {
        a_i[i] = a[i];
    }
    for( i = 0; i < (m-1); i++ ) {
        tau_i[i] = tau[i];
    }
    for( i = 0; i < ldc*n; i++ ) {
        c_i[i] = c_save[i];
    }
    for( i = 0; i < lwork; i++ ) {
        work_i[i] = work[i];
    }
    info_i = LAPACKE_zunmtr( LAPACK_COL_MAJOR, side_i, uplo_i, trans_i, m_i,
                             n_i, a_i, lda_i, tau_i, c_i, ldc_i );

    failed = compare_zunmtr( c, c_i, info, info_i, ldc, n );
    if( failed == 0 ) {
        printf( "PASSED: column-major high-level interface to zunmtr\n" );
    } else {
        printf( "FAILED: column-major high-level interface to zunmtr\n" );
    }

    /* Initialize input data, call the row-major middle-level
     * interface to LAPACK routine and check the results */
    for( i = 0; i < lda*m; i++ ) {
        a_i[i] = a[i];
    }
    for( i = 0; i < (m-1); i++ ) {
        tau_i[i] = tau[i];
    }
    for( i = 0; i < ldc*n; i++ ) {
        c_i[i] = c_save[i];
    }
    for( i = 0; i < lwork; i++ ) {
        work_i[i] = work[i];
    }

    LAPACKE_zge_trans( LAPACK_COL_MAJOR, r, r, a_i, lda, a_r, r+2 );
    LAPACKE_zge_trans( LAPACK_COL_MAJOR, m, n, c_i, ldc, c_r, n+2 );
    info_i = LAPACKE_zunmtr_work( LAPACK_ROW_MAJOR, side_i, uplo_i, trans_i,
                                  m_i, n_i, a_r, lda_r, tau_i, c_r, ldc_r,
                                  work_i, lwork_i );

    LAPACKE_zge_trans( LAPACK_ROW_MAJOR, m, n, c_r, n+2, c_i, ldc );

    failed = compare_zunmtr( c, c_i, info, info_i, ldc, n );
    if( failed == 0 ) {
        printf( "PASSED: row-major middle-level interface to zunmtr\n" );
    } else {
        printf( "FAILED: row-major middle-level interface to zunmtr\n" );
    }

    /* Initialize input data, call the row-major high-level
     * interface to LAPACK routine and check the results */
    for( i = 0; i < lda*m; i++ ) {
        a_i[i] = a[i];
    }
    for( i = 0; i < (m-1); i++ ) {
        tau_i[i] = tau[i];
    }
    for( i = 0; i < ldc*n; i++ ) {
        c_i[i] = c_save[i];
    }
    for( i = 0; i < lwork; i++ ) {
        work_i[i] = work[i];
    }

    /* Init row_major arrays */
    LAPACKE_zge_trans( LAPACK_COL_MAJOR, r, r, a_i, lda, a_r, r+2 );
    LAPACKE_zge_trans( LAPACK_COL_MAJOR, m, n, c_i, ldc, c_r, n+2 );
    info_i = LAPACKE_zunmtr( LAPACK_ROW_MAJOR, side_i, uplo_i, trans_i, m_i,
                             n_i, a_r, lda_r, tau_i, c_r, ldc_r );

    LAPACKE_zge_trans( LAPACK_ROW_MAJOR, m, n, c_r, n+2, c_i, ldc );

    failed = compare_zunmtr( c, c_i, info, info_i, ldc, n );
    if( failed == 0 ) {
        printf( "PASSED: row-major high-level interface to zunmtr\n" );
    } else {
        printf( "FAILED: row-major high-level interface to zunmtr\n" );
    }

    /* Release memory */
    if( a != NULL ) {
        LAPACKE_free( a );
    }
    if( a_i != NULL ) {
        LAPACKE_free( a_i );
    }
    if( a_r != NULL ) {
        LAPACKE_free( a_r );
    }
    if( tau != NULL ) {
        LAPACKE_free( tau );
    }
    if( tau_i != NULL ) {
        LAPACKE_free( tau_i );
    }
    if( c != NULL ) {
        LAPACKE_free( c );
    }
    if( c_i != NULL ) {
        LAPACKE_free( c_i );
    }
    if( c_r != NULL ) {
        LAPACKE_free( c_r );
    }
    if( c_save != NULL ) {
        LAPACKE_free( c_save );
    }
    if( work != NULL ) {
        LAPACKE_free( work );
    }
    if( work_i != NULL ) {
        LAPACKE_free( work_i );
    }

    return 0;
}

/* Auxiliary function: zunmtr scalar parameters initialization */
static void init_scalars_zunmtr( char *side, char *uplo, char *trans,
                                 lapack_int *m, lapack_int *n, lapack_int *lda,
                                 lapack_int *ldc, lapack_int *lwork )
{
    *side = 'L';
    *uplo = 'L';
    *trans = 'N';
    *m = 4;
    *n = 2;
    *lda = 8;
    *ldc = 8;
    *lwork = 512;

    return;
}

/* Auxiliary functions: zunmtr array parameters initialization */
static void init_a( lapack_int size, lapack_complex_double *a ) {
    lapack_int i;
    for( i = 0; i < size; i++ ) {
        a[i] = lapack_make_complex_double( 0.0, 0.0 );
    }
    a[0] = lapack_make_complex_double( -2.27999999999999980e+000,
                                       0.00000000000000000e+000 );
    a[8] = lapack_make_complex_double( 0.00000000000000000e+000,
                                       0.00000000000000000e+000 );
    a[16] = lapack_make_complex_double( 0.00000000000000000e+000,
                                        0.00000000000000000e+000 );
    a[24] = lapack_make_complex_double( 0.00000000000000000e+000,
                                        0.00000000000000000e+000 );
    a[1] = lapack_make_complex_double( -4.33845594653212970e+000,
                                       0.00000000000000000e+000 );
    a[9] = lapack_make_complex_double( -1.28456981649329280e-001,
                                       0.00000000000000000e+000 );
    a[17] = lapack_make_complex_double( 0.00000000000000000e+000,
                                        0.00000000000000000e+000 );
    a[25] = lapack_make_complex_double( 0.00000000000000000e+000,
                                        0.00000000000000000e+000 );
    a[2] = lapack_make_complex_double( 3.27860676092192380e-001,
                                       -1.25122609226443690e-001 );
    a[10] = lapack_make_complex_double( -2.02259457862261720e+000,
                                        0.00000000000000000e+000 );
    a[18] = lapack_make_complex_double( -1.66593253752407190e-001,
                                        0.00000000000000000e+000 );
    a[26] = lapack_make_complex_double( 0.00000000000000000e+000,
                                        0.00000000000000000e+000 );
    a[3] = lapack_make_complex_double( -1.41256563750694670e-001,
                                       -3.66636483973957040e-001 );
    a[11] = lapack_make_complex_double( -3.08321908008089010e-001,
                                        1.76322636472677850e-001 );
    a[19] = lapack_make_complex_double( -1.80232297833873440e+000,
                                        0.00000000000000000e+000 );
    a[27] = lapack_make_complex_double( -1.92494976459826360e+000,
                                        0.00000000000000000e+000 );
}
static void init_tau( lapack_int size, lapack_complex_double *tau ) {
    lapack_int i;
    for( i = 0; i < size; i++ ) {
        tau[i] = lapack_make_complex_double( 0.0, 0.0 );
    }
    tau[0] = lapack_make_complex_double( 1.41028421676675380e+000,
                                         4.67908404514893240e-001 );
    tau[1] = lapack_make_complex_double( 1.30242036943477490e+000,
                                         7.85332074252958030e-001 );
    tau[2] = lapack_make_complex_double( 1.09397371592308160e+000,
                                         -9.95574678623159850e-001 );
}
static void init_c( lapack_int size, lapack_complex_double *c ) {
    lapack_int i;
    for( i = 0; i < size; i++ ) {
        c[i] = lapack_make_complex_double( 0.0, 0.0 );
    }
    c[0] = lapack_make_complex_double( 7.29894574391705130e-001,
                                       0.00000000000000000e+000 );
    c[8] = lapack_make_complex_double( -2.59544973387760720e-001,
                                       0.00000000000000000e+000 );
    c[1] = lapack_make_complex_double( 6.25877780555793130e-001,
                                       0.00000000000000000e+000 );
    c[9] = lapack_make_complex_double( -4.32549625865536950e-002,
                                       0.00000000000000000e+000 );
    c[2] = lapack_make_complex_double( 2.51344947364408430e-001,
                                       0.00000000000000000e+000 );
    c[10] = lapack_make_complex_double( 4.95247410182067920e-001,
                                        0.00000000000000000e+000 );
    c[3] = lapack_make_complex_double( 1.11160386444491490e-001,
                                       0.00000000000000000e+000 );
    c[11] = lapack_make_complex_double( 8.27946506550234270e-001,
                                        0.00000000000000000e+000 );
}
static void init_work( lapack_int size, lapack_complex_double *work ) {
    lapack_int i;
    for( i = 0; i < size; i++ ) {
        work[i] = lapack_make_complex_double( 0.0, 0.0 );
    }
}

/* Auxiliary function: C interface to zunmtr results check */
/* Return value: 0 - test is passed, non-zero - test is failed */
static int compare_zunmtr( lapack_complex_double *c, lapack_complex_double *c_i,
                           lapack_int info, lapack_int info_i, lapack_int ldc,
                           lapack_int n )
{
    lapack_int i;
    int failed = 0;
    for( i = 0; i < ldc*n; i++ ) {
        failed += compare_complex_doubles(c[i],c_i[i]);
    }
    failed += (info == info_i) ? 0 : 1;
    if( info != 0 || info_i != 0 ) {
        printf( "info=%d, info_i=%d\n",(int)info,(int)info_i );
    }

    return failed;
}
