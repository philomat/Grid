/*************************************************************************************

Grid physics library, www.github.com/paboyle/Grid

Source file: ./lib/qcd/action/fermion/WilsonKernels.h

Copyright (C) 2015

Author: Peter Boyle <pabobyle@ph.ed.ac.uk>
Author: Peter Boyle <paboyle@ph.ed.ac.uk>
Author: paboyle <paboyle@ph.ed.ac.uk>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

See the full license in the file "LICENSE" in the top level distribution
directory
*************************************************************************************/
			   /*  END LEGAL */
#ifndef GRID_QCD_DHOP_H
#define GRID_QCD_DHOP_H

NAMESPACE_BEGIN(Grid);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper routines that implement Wilson stencil for a single site.
// Common to both the WilsonFermion and WilsonFermion5D
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WilsonKernelsStatic { 
public:
  enum { OptGeneric, OptHandUnroll, OptInlineAsm };
  enum { CommsAndCompute, CommsThenCompute };
  static int Opt;  
  static int Comms;
};
 
template<class Impl> class WilsonKernels : public FermionOperator<Impl> { 
public:

  INHERIT_IMPL_TYPES(Impl);
  typedef FermionOperator<Impl> Base;

  typedef typename ViewMap<FermionField>::Type      FermionFieldView;
  typedef typename ViewMap<DoubledGaugeField>::Type DoubledGaugeFieldView;
   
public:
   
  template <bool EnableBool = true> static accelerator
  typename std::enable_if<Impl::Dimension == 3 && Nc == 3 &&EnableBool, void>::type
  DhopSite(int Opt,typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
	   int sF, int sU, int Ls, int Nsite, const FermionFieldView &in, FermionFieldView &out,int interior=1,int exterior=1) 
  {
    bgq_l1p_optimisation(1);
    switch(Opt) {
#if defined(AVX512) || defined (QPX)
    case WilsonKernelsStatic::OptInlineAsm:
      if(interior&&exterior) WilsonKernels<Impl>::AsmDhopSite   (st,U,buf,sF,sU,Ls,Nsite,in,out);
      else if (interior)     WilsonKernels<Impl>::AsmDhopSiteInt(st,U,buf,sF,sU,Ls,Nsite,in,out);
      else if (exterior)     WilsonKernels<Impl>::AsmDhopSiteExt(st,U,buf,sF,sU,Ls,Nsite,in,out);
      else assert(0);
      break;
#endif
    case WilsonKernelsStatic::OptHandUnroll:
      for (int site = 0; site < Nsite; site++) {
	for (int s = 0; s < Ls; s++) {
	  if(interior&&exterior) WilsonKernels<Impl>::HandDhopSite(st,U,buf,sF,sU,in,out);
	  else if (interior)     WilsonKernels<Impl>::HandDhopSiteInt(st,U,buf,sF,sU,in,out);
	  else if (exterior)     WilsonKernels<Impl>::HandDhopSiteExt(st,U,buf,sF,sU,in,out);
	  sF++;
	}
	sU++;
      }
      break;
    case WilsonKernelsStatic::OptGeneric:
      for (int site = 0; site < Nsite; site++) {
	for (int s = 0; s < Ls; s++) {
	  if(interior&&exterior) WilsonKernels<Impl>::GenericDhopSite(st,U,buf,sF,sU,in,out);
	  else if (interior)     WilsonKernels<Impl>::GenericDhopSiteInt(st,U,buf,sF,sU,in,out);
	  else if (exterior)     WilsonKernels<Impl>::GenericDhopSiteExt(st,U,buf,sF,sU,in,out);
	  else assert(0);
	  sF++;
	}
	sU++;
      }
      break;
    default:
      assert(0);
    }
    bgq_l1p_optimisation(0);
  }
     
  template <bool EnableBool = true> static accelerator
  typename std::enable_if<(Impl::Dimension != 3 || (Impl::Dimension == 3 && Nc != 3)) && EnableBool, void>::type
  DhopSite(int Opt, typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
	   int sF, int sU, int Ls, int Nsite, const FermionFieldView &in, FermionFieldView &out,int interior=1,int exterior=1 ) {
    // no kernel choice  
    for (int site = 0; site < Nsite; site++) {
      for (int s = 0; s < Ls; s++) {
	if(interior&&exterior) WilsonKernels<Impl>::GenericDhopSite(st,U,buf,sF,sU,in,out);
	else if (interior)     WilsonKernels<Impl>::GenericDhopSiteInt(st,U,buf,sF,sU,in,out);
	else if (exterior)     WilsonKernels<Impl>::GenericDhopSiteExt(st,U,buf,sF,sU,in,out);
	else assert(0);
	sF++;
      }
      sU++;
    }
  }
     
  template <bool EnableBool = true> static accelerator
  typename std::enable_if<Impl::Dimension == 3 && Nc == 3 && EnableBool,void>::type
  DhopSiteDag(int Opt, typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
	      int sF, int sU, int Ls, int Nsite, const FermionFieldView &in, FermionFieldView &out,int interior=1,int exterior=1) 
  {
    bgq_l1p_optimisation(1);
    switch(Opt) {
#if defined(AVX512) || defined (QPX)
    case WilsonKernelsStatic::OptInlineAsm:
      if(interior&&exterior) WilsonKernels<Impl>::AsmDhopSiteDag   (st,U,buf,sF,sU,Ls,Nsite,in,out);
      else if (interior)     WilsonKernels<Impl>::AsmDhopSiteDagInt(st,U,buf,sF,sU,Ls,Nsite,in,out);
      else if (exterior)     WilsonKernels<Impl>::AsmDhopSiteDagExt(st,U,buf,sF,sU,Ls,Nsite,in,out);
      else assert(0);
      break;
#endif
    case WilsonKernelsStatic::OptHandUnroll:
      for (int site = 0; site < Nsite; site++) {
	for (int s = 0; s < Ls; s++) {
	  if(interior&&exterior) WilsonKernels<Impl>::HandDhopSiteDag(st,U,buf,sF,sU,in,out);
	  else if (interior)     WilsonKernels<Impl>::HandDhopSiteDagInt(st,U,buf,sF,sU,in,out);
	  else if (exterior)     WilsonKernels<Impl>::HandDhopSiteDagExt(st,U,buf,sF,sU,in,out);
	  else assert(0);
	  sF++;
	}
	sU++;
      }
      break;
    case WilsonKernelsStatic::OptGeneric:
      for (int site = 0; site < Nsite; site++) {
	for (int s = 0; s < Ls; s++) {
	  if(interior&&exterior) WilsonKernels<Impl>::GenericDhopSiteDag(st,U,buf,sF,sU,in,out);
	  else if (interior)     WilsonKernels<Impl>::GenericDhopSiteDagInt(st,U,buf,sF,sU,in,out);
	  else if (exterior)     WilsonKernels<Impl>::GenericDhopSiteDagExt(st,U,buf,sF,sU,in,out);
	  else assert(0);
	  sF++;
	}
	sU++;
      }
      break;
    default:
      assert(0);
    }
    bgq_l1p_optimisation(0);
  }

  template <bool EnableBool = true> static accelerator
  typename std::enable_if<(Impl::Dimension != 3 || (Impl::Dimension == 3 && Nc != 3)) && EnableBool,void>::type
  DhopSiteDag(int Opt,typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U,SiteHalfSpinor * buf,
	      int sF, int sU, int Ls, int Nsite, const FermionFieldView &in, FermionFieldView &out,int interior=1,int exterior=1) {

    for (int site = 0; site < Nsite; site++) {
      for (int s = 0; s < Ls; s++) {
	if(interior&&exterior) WilsonKernels<Impl>::GenericDhopSiteDag(st,U,buf,sF,sU,in,out);
	else if (interior)     WilsonKernels<Impl>::GenericDhopSiteDagInt(st,U,buf,sF,sU,in,out);
	else if (exterior)     WilsonKernels<Impl>::GenericDhopSiteDagExt(st,U,buf,sF,sU,in,out);
	else assert(0);
	sF++;
      }
      sU++;
    }
  }

  static accelerator void DhopDirK(typename StencilImpl::View_type &st, DoubledGaugeFieldView &U,SiteHalfSpinor * buf,
				   int sF, int sU, const FermionFieldView &in, FermionFieldView &out, int dirdisp, int gamma);
      
  //////////////////////////////////////////////////////////////////////////////
  // Utilities for inserting Wilson conserved current.
  //////////////////////////////////////////////////////////////////////////////
  static void ContractConservedCurrentSiteFwd(const SitePropagator &q_in_1,
                                       const SitePropagator &q_in_2,
                                       SitePropagator &q_out,
                                       DoubledGaugeFieldView &U,
                                       unsigned int sU,
                                       unsigned int mu,
                                       bool switch_sign = false);
  static void ContractConservedCurrentSiteBwd(const SitePropagator &q_in_1,
                                       const SitePropagator &q_in_2,
                                       SitePropagator &q_out,
                                       DoubledGaugeFieldView &U,
                                       unsigned int sU,
                                       unsigned int mu,
                                       bool switch_sign = false);
  static void SeqConservedCurrentSiteFwd(const SitePropagator &q_in, 
                                  SitePropagator &q_out,
                                  DoubledGaugeFieldView &U,
                                  unsigned int sU,
                                  unsigned int mu,
                                  vInteger t_mask,
                                  bool switch_sign = false);
  static void SeqConservedCurrentSiteBwd(const SitePropagator &q_in,
                                  SitePropagator &q_out,
                                  DoubledGaugeFieldView &U,
                                  unsigned int sU,
                                  unsigned int mu,
                                  vInteger t_mask,
                                  bool switch_sign = false);

private:
  // Specialised variants
  static accelerator void GenericDhopSite(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
				   int sF, int sU, const FermionFieldView &in, FermionFieldView &out);
      
  static accelerator void GenericDhopSiteDag(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
				      int sF, int sU, const FermionFieldView &in, FermionFieldView &out);
  
  static accelerator void GenericDhopSiteInt(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
				      int sF, int sU, const FermionFieldView &in, FermionFieldView &out);
      
  static accelerator void GenericDhopSiteDagInt(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
					 int sF, int sU, const FermionFieldView &in, FermionFieldView &out);
  
  static accelerator void GenericDhopSiteExt(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
				      int sF, int sU, const FermionFieldView &in, FermionFieldView &out);
      
  static accelerator void GenericDhopSiteDagExt(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
					 int sF, int sU, const FermionFieldView &in, FermionFieldView &out);
  
  static accelerator void AsmDhopSite(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
			       int sF, int sU, int Ls, int Nsite, const FermionFieldView &in,FermionFieldView &out);
  
  static accelerator void AsmDhopSiteDag(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
				  int sF, int sU, int Ls, int Nsite, const FermionFieldView &in, FermionFieldView &out);
  
  static accelerator void AsmDhopSiteInt(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
				  int sF, int sU, int Ls, int Nsite, const FermionFieldView &in,FermionFieldView &out);
  
  static accelerator void AsmDhopSiteDagInt(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
				     int sF, int sU, int Ls, int Nsite, const FermionFieldView &in, FermionFieldView &out);
  
  static accelerator void AsmDhopSiteExt(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
				  int sF, int sU, int Ls, int Nsite, const FermionFieldView &in,FermionFieldView &out);
  
  static accelerator void AsmDhopSiteDagExt(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
				     int sF, int sU, int Ls, int Nsite, const FermionFieldView &in, FermionFieldView &out);
  

  static accelerator void HandDhopSite(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
				int sF, int sU, const FermionFieldView &in, FermionFieldView &out);
  
  static accelerator void HandDhopSiteDag(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
				   int sF, int sU, const FermionFieldView &in, FermionFieldView &out);
  
  static accelerator void HandDhopSiteInt(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
				   int sF, int sU, const FermionFieldView &in, FermionFieldView &out);
  
  static accelerator void HandDhopSiteDagInt(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
				      int sF, int sU, const FermionFieldView &in, FermionFieldView &out);
  
  static accelerator void HandDhopSiteExt(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
				   int sF, int sU, const FermionFieldView &in, FermionFieldView &out);
  
  static accelerator void HandDhopSiteDagExt(typename StencilImpl::View_type &st,  DoubledGaugeFieldView &U, SiteHalfSpinor * buf,
				      int sF, int sU, const FermionFieldView &in, FermionFieldView &out);
  
public:
  
  WilsonKernels(const ImplParams &p = ImplParams());

};
    
NAMESPACE_END(Grid);

#endif
