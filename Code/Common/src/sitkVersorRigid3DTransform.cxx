/*=========================================================================
*
*  Copyright Insight Software Consortium
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*         http://www.apache.org/licenses/LICENSE-2.0.txt
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*=========================================================================*/
#include "sitkVersorRigid3DTransform.h"
#include "sitkTransformHelper.hxx"

#include "itkVersorRigid3DTransform.h"

namespace itk
{
namespace simple
{

VersorRigid3DTransform::~VersorRigid3DTransform()
{
}

// construct identity
VersorRigid3DTransform::VersorRigid3DTransform()
  : Transform(3, sitkVersorRigid)
{
  Self::InternalInitialization(Self::GetITKBase());
}

VersorRigid3DTransform::VersorRigid3DTransform( const VersorRigid3DTransform &arg )
  : Transform(arg)
{
  Self::InternalInitialization(Self::GetITKBase());
}

VersorRigid3DTransform::VersorRigid3DTransform( const Transform & arg )
  : Transform(arg)
{
  Self::InternalInitialization(Self::GetITKBase());
}

VersorRigid3DTransform::VersorRigid3DTransform( const std::vector< double > &versor,
                                                const std::vector< double > &translation,
                                                const std::vector< double> &fixedCenter )
  : Transform(3, sitkVersorRigid)
{
  Self::InternalInitialization(Self::GetITKBase());

  this->SetCenter(fixedCenter);
  this->SetRotation(versor);
  this->SetTranslation(translation);
}

VersorRigid3DTransform::VersorRigid3DTransform( const std::vector< double > &axis, double angle,
                                                const std::vector< double > &translation,
                                                const std::vector< double> &fixedCenter )
  : Transform(3, sitkVersorRigid)
{
  Self::InternalInitialization(Self::GetITKBase());

  this->SetCenter(fixedCenter);
  this->SetRotation(axis, angle);
  this->SetTranslation(translation);
}

VersorRigid3DTransform &VersorRigid3DTransform::operator=( const VersorRigid3DTransform &arg )
{
  Superclass::operator=(arg);
  return *this;
}


/** fixed parameter */
VersorRigid3DTransform::Self &VersorRigid3DTransform::SetCenter(const std::vector<double> &params)
{
  this->MakeUnique();
  this->m_pfSetCenter(params);
  return *this;
}

std::vector<double> VersorRigid3DTransform::GetCenter( ) const
{
  return this->m_pfGetCenter();
}


VersorRigid3DTransform::Self &VersorRigid3DTransform::SetRotation(const std::vector<double> &versor)
{
  this->MakeUnique();
  this->m_pfSetRotation1(versor);
  return *this;
}

VersorRigid3DTransform::Self &VersorRigid3DTransform::SetRotation(const std::vector<double> &axis,  double angle)
{
  this->MakeUnique();
  this->m_pfSetRotation2(axis, angle);
  return *this;
}

std::vector<double>  VersorRigid3DTransform::GetVersor() const
{
  return this->m_pfGetVersor();
}

VersorRigid3DTransform::Self &VersorRigid3DTransform::SetTranslation(const std::vector<double> &params)
{
  this->MakeUnique();
  this->m_pfSetTranslation(params);
  return *this;
}

std::vector<double> VersorRigid3DTransform::GetTranslation( ) const
{
  return this->m_pfGetTranslation();
}

VersorRigid3DTransform::Self &VersorRigid3DTransform::Translate(const std::vector<double> &offset)
{
  this->MakeUnique();
  this->m_pfTranslate(offset);
  return *this;
}

std::vector<double> VersorRigid3DTransform::GetMatrix( ) const
{
  return this->m_pfGetMatrix();
}

VersorRigid3DTransform::Self &VersorRigid3DTransform::SetMatrix(const std::vector<double> &params, double tolerance)
{
  this->MakeUnique();
  this->m_pfSetMatrix(params,tolerance);
  return *this;
}

void VersorRigid3DTransform::SetPimpleTransform( PimpleTransformBase *pimpleTransform )
{
  Superclass::SetPimpleTransform(pimpleTransform);
  Self::InternalInitialization(this->GetITKBase());
}

void VersorRigid3DTransform::InternalInitialization(itk::TransformBase *transform)
{

  typedef itk::VersorRigid3DTransform<double> TransformType;
  TransformType *t = dynamic_cast<TransformType*>(transform);

  // explicitly remove all function pointer with reference to prior transform
  this->m_pfSetCenter = nullptr;
  this->m_pfGetCenter = nullptr;
  this->m_pfSetTranslation = nullptr;
  this->m_pfGetTranslation = nullptr;
  this->m_pfSetRotation1 = nullptr;
  this->m_pfSetRotation2 = nullptr;
  this->m_pfGetVersor = nullptr;
  this->m_pfTranslate = nullptr;
  this->m_pfGetMatrix = nullptr;
  this->m_pfSetMatrix = nullptr;

  if (t && (typeid(*t) == typeid(TransformType)))
    {
    this->InternalInitialization(t);
    return;
    }
  else
    {
    sitkExceptionMacro("Transform is not of type " << this->GetName() << "!" );
    }
}


template<class TransformType>
void VersorRigid3DTransform::InternalInitialization(TransformType *t)
{

  SITK_TRANSFORM_SET_MPF(Center, typename TransformType::InputPointType, double);
  SITK_TRANSFORM_SET_MPF(Translation, typename TransformType::OutputVectorType, double);
  SITK_TRANSFORM_SET_MPF_GetMatrix();
  SITK_TRANSFORM_SET_MPF_SetMatrix();

  void  (TransformType::*pfSetRotation1) (const typename TransformType::VersorType &) = &TransformType::SetRotation;
  this->m_pfSetRotation1 = std::bind(pfSetRotation1,t,std::bind(&sitkSTLVectorToITKVersor<double, double>,std::placeholders::_1));

  typename TransformType::OutputVectorType (*pfSTLVectorToITK)(const std::vector<double> &) = &sitkSTLVectorToITK<typename TransformType::OutputVectorType, double>;
  void  (TransformType::*pfSetRotation2) (const typename TransformType::AxisType &, double) = &TransformType::SetRotation;
  this->m_pfSetRotation2 = std::bind(pfSetRotation2,t,std::bind(pfSTLVectorToITK,std::placeholders::_1),std::placeholders::_2);

  this->m_pfGetVersor  = std::bind(&sitkITKVersorToSTL<double, double>,std::bind(&TransformType::GetVersor,t));

  // pre argument has no effect
  this->m_pfTranslate = std::bind(&TransformType::Translate,t,std::bind(pfSTLVectorToITK,std::placeholders::_1), false);
}

}
}
