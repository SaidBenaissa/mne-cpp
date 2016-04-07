//=============================================================================================================
/**
* @file     fiff_coord_trans.cpp
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     July, 2012
*
* @section  LICENSE
*
* Copyright (C) 2012, Christoph Dinh and Matti Hamalainen. All rights reserved.
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
* @brief    Implementation of the FiffCoordTrans Class.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "fiff_coord_trans.h"

#include "fiff_stream.h"
#include "fiff_tag.h"
#include "fiff_dir_tree.h"


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace FIFFLIB;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

FiffCoordTrans::FiffCoordTrans()
: from(-1)
, to(-1)
, trans(MatrixXf::Identity(4,4))
, invtrans(MatrixXf::Identity(4,4))
{
}


//*************************************************************************************************************

FiffCoordTrans::FiffCoordTrans(QIODevice &p_IODevice)
: from(-1)
, to(-1)
, trans(MatrixXf::Identity(4,4))
, invtrans(MatrixXf::Identity(4,4))
{
    if(!read(p_IODevice, *this))
    {
        printf("\tCoordindate transform not found.\n");//ToDo Throw here
        return;
    }
}


//*************************************************************************************************************

FiffCoordTrans::FiffCoordTrans(const FiffCoordTrans &p_FiffCoordTrans)
: from(p_FiffCoordTrans.from)
, to(p_FiffCoordTrans.to)
, trans(p_FiffCoordTrans.trans)
, invtrans(p_FiffCoordTrans.invtrans)
{
}


//*************************************************************************************************************

FiffCoordTrans::~FiffCoordTrans()
{
}


//*************************************************************************************************************

void FiffCoordTrans::clear()
{
    from = -1;
    to = -1;
    trans.setIdentity();
    invtrans.setIdentity();
}


//*************************************************************************************************************

bool FiffCoordTrans::invert_transform()
{
    fiff_int_t from_new = this->to;
    this->to    = this->from;
    this->from  = from_new;
    this->trans = this->trans.inverse();
    this->invtrans = this->invtrans.inverse();

    return true;
}


//*************************************************************************************************************

bool FiffCoordTrans::read(QIODevice& p_IODevice, FiffCoordTrans& p_Trans)
{
    FiffStream::SPtr t_pStream(new FiffStream(&p_IODevice));
    FiffDirTree t_Tree;
    QList<FiffDirEntry> t_Dir;

    printf("Reading coordinate transform from %s...\n", t_pStream->streamName().toUtf8().constData());
    if(!t_pStream->open(t_Tree, t_Dir))
        return false;

    //
    //   Locate and read the coordinate transformation
    //
    FiffTag::SPtr t_pTag;
    bool success = false;

    //
    //   Get the MRI <-> head coordinate transformation
    //
    for ( qint32 k = 0; k < t_Dir.size(); ++k )
    {
        if ( t_Dir[k].kind == FIFF_COORD_TRANS )
        {
            FiffTag::read_tag(t_pStream.data(),t_pTag,t_Dir[k].pos);
            p_Trans = t_pTag->toCoordTrans();
            success = true;
        }
    }

    return success;
}
