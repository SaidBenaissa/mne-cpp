//=============================================================================================================
/**
* @file     mne_cov_matrix.h
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     January, 2017
*
* @section  LICENSE
*
* Copyright (C) 2017, Christoph Dinh and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    MneCovMatrix class declaration.
*
*/

#ifndef MNECOVMATRIX_H
#define MNECOVMATRIX_H

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "../inverse_global.h"


//*************************************************************************************************************
//=============================================================================================================
// Eigen INCLUDES
//=============================================================================================================

#include <Eigen/Core>


//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================

#include <QSharedPointer>




/*
 * The class field in mneCovMatrix can have these values
 */
#define MNE_COV_CH_UNKNOWN  -1	/* No idea */
#define MNE_COV_CH_MEG_MAG   0  /* Axial gradiometer or magnetometer [T] */
#define MNE_COV_CH_MEG_GRAD  1  /* Planar gradiometer [T/m] */
#define MNE_COV_CH_EEG       2  /* EEG [V] */



//*************************************************************************************************************
//=============================================================================================================
// DEFINE NAMESPACE INVERSELIB
//=============================================================================================================

namespace INVERSELIB
{

//*************************************************************************************************************
//=============================================================================================================
// FORWARD DECLARATIONS
//=============================================================================================================

class FiffSparseMatrix;
class MneProjOp;
class MneSssData;


//=============================================================================================================
/**
* Implements an MNE Covariance Matrix (Replaces *mneCovMatrix,mneCovMatrixRec; struct of MNE-C mne_types.h).
*
* @brief Covariance matrix storage
*/
class INVERSESHARED_EXPORT MneCovMatrix
{
public:
    typedef QSharedPointer<MneCovMatrix> SPtr;              /**< Shared pointer type for MneCovMatrix. */
    typedef QSharedPointer<const MneCovMatrix> ConstSPtr;   /**< Const shared pointer type for MneCovMatrix. */

    //=========================================================================================================
    /**
    * Constructs the MNE Covariance Matrix
    * Refactored: new_cov (mne_cov_matrix.c)
    */
    MneCovMatrix(int p_kind, int p_ncov, char **p_names, double *p_cov, double *p_cov_diag, FiffSparseMatrix* p_cov_sparse);

    //=========================================================================================================
    /**
    * Destroys the MNE Covariance Matrix
    * Refactored: mne_free_cov (mne_cov_matrix.c)
    */
    ~MneCovMatrix();



    static MneCovMatrix* mne_dup_cov(MneCovMatrix* c);





    static MneCovMatrix* mne_new_cov_dense(int    kind,
                                   int    ncov,
                                   char   **names,
                                   double *cov)
    {
        return new MneCovMatrix(kind,ncov,names,cov,NULL,NULL);
    }


    static MneCovMatrix* mne_new_cov_diag(int    kind,
                                  int    ncov,
                                  char   **names,
                                  double *cov_diag)
    {
        return new MneCovMatrix(kind,ncov,names,NULL,cov_diag,NULL);
    }


    static MneCovMatrix* mne_new_cov_sparse(    int kind,
                                                int ncov,
                                                char **names,
                                    FiffSparseMatrix* cov_sparse)
    {
        return new MneCovMatrix(kind,ncov,names,NULL,NULL,cov_sparse);
    }


    static MneCovMatrix* mne_new_cov(   int kind,
                                        int ncov,
                                        char    **names,
                                        double      *cov,
                                        double *cov_diag)
    {
        return new MneCovMatrix(kind,ncov,names,cov,cov_diag,NULL);
    }




    static int mne_is_diag_cov(MneCovMatrix* c);



    static int mne_add_inv_cov(MneCovMatrix* c);







    static int condition_cov(MneCovMatrix* c, float rank_threshold, int use_rank);









    static int mne_decompose_eigen_cov_small(MneCovMatrix* c,float small, int use_rank);


    static int mne_decompose_eigen_cov(MneCovMatrix* c);










private:

    static int mne_lt_packed_index(int j, int k);




public:
    int         kind;                           /* Sensor or source covariance */
    int         ncov;                           /* Dimension */
    int         nfree;                          /* Number of degrees of freedom */
    int         nproj;                          /* Number of dimensions projected out */
    int         nzero;                          /* Number of zero or small eigenvalues */
    char        **names;                        /* Names of the entries (optional) */
    double      *cov;                           /* Covariance matrix in packed representation (lower triangle) */
    double      *cov_diag;                      /* Diagonal covariance matrix */
    FiffSparseMatrix* cov_sparse;   /* A sparse covariance matrix (Note: data are floats in this which is an inconsistency) */
    double      *lambda;                /* Eigenvalues of cov */
    double      *inv_lambda;            /* Inverses of the square roots of the eigenvalues of cov */
    float       **eigen;                /* Eigenvectors of cov */
    double      *chol;                  /* Cholesky decomposition */
    MneProjOp*  proj;       /* The projection which was active when this matrix was computed */
    MneSssData* sss;        /* The SSS data present in the associated raw data file */
    int         *ch_class;              /* This will allow grouping of channels for regularization (MEG [T/m], MEG [T], EEG [V] */
    char        **bads;                 /* Which channels were designated bad when this noise covariance matrix was computed? */
    int         nbad;                   /* How many of them */

// ### OLD STRUCT ###
//typedef struct {                                /* Covariance matrix storage */
//    int         kind;                           /* Sensor or source covariance */
//    int         ncov;                           /* Dimension */
//    int         nfree;                          /* Number of degrees of freedom */
//    int         nproj;                          /* Number of dimensions projected out */
//    int         nzero;                          /* Number of zero or small eigenvalues */
//    char        **names;                        /* Names of the entries (optional) */
//    double      *cov;                           /* Covariance matrix in packed representation (lower triangle) */
//    double      *cov_diag;                      /* Diagonal covariance matrix */
//    INVERSELIB::FiffSparseMatrix* cov_sparse;   /* A sparse covariance matrix (Note: data are floats in this which is an inconsistency) */
//    double      *lambda;                /* Eigenvalues of cov */
//    double      *inv_lambda;            /* Inverses of the square roots of the eigenvalues of cov */
//    float       **eigen;                /* Eigenvectors of cov */
//    double      *chol;                  /* Cholesky decomposition */
//    INVERSELIB::MneProjOp*  proj;       /* The projection which was active when this matrix was computed */
//    INVERSELIB::MneSssData* sss;        /* The SSS data present in the associated raw data file */
//    int         *ch_class;              /* This will allow grouping of channels for regularization (MEG [T/m], MEG [T], EEG [V] */
//    char        **bads;                 /* Which channels were designated bad when this noise covariance matrix was computed? */
//    int         nbad;                   /* How many of them */
//} *mneCovMatrix,mneCovMatrixRec;
};

//*************************************************************************************************************
//=============================================================================================================
// INLINE DEFINITIONS
//=============================================================================================================

} // NAMESPACE INVERSELIB

#endif // MNECOVMATRIX_H
