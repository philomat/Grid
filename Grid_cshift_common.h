#ifndef _GRID_CSHIFT_COMMON_H_
#define _GRID_CSHIFT_COMMON_H_

//////////////////////////////////////////////////////
// Gather for when there is no need to SIMD split
//////////////////////////////////////////////////////
friend void Gather_plane_simple (Lattice<vobj> &rhs,std::vector<vobj,alignedAllocator<vobj> > &buffer,             int dimension,int plane,int cbmask)
{
  int rd = rhs._grid->_rdimensions[dimension];

  printf("Gather_plane_simple buf size %d rhs size %d \n",buffer.size(),rhs._odata.size());fflush(stdout);
  if ( !rhs._grid->CheckerBoarded(dimension) ) {

    int so  = plane*rhs._grid->_ostride[dimension]; // base offset for start of plane 
    int o   = 0;                                    // relative offset to base within plane
    int bo  = 0;                                    // offset in buffer

    // Simple block stride gather of SIMD objects
#pragma omp parallel for collapse(2)
    for(int n=0;n<rhs._grid->_slice_nblock[dimension];n++){
      for(int b=0;b<rhs._grid->_slice_block[dimension];b++){
	printf("Gather_plane_simple %d ; %d %d %d\n",bo,so,o,b);fflush(stdout);
	buffer[bo++]=rhs._odata[so+o+b];
      }
      o +=rhs._grid->_slice_stride[dimension];
    }

  } else { 

    int so  = plane*rhs._grid->_ostride[dimension]; // base offset for start of plane 
    int o   = 0;                                      // relative offset to base within plane
    int bo  = 0;                                      // offset in buffer

#pragma omp parallel for collapse(2)
    for(int n=0;n<rhs._grid->_slice_nblock[dimension];n++){
      for(int b=0;b<rhs._grid->_slice_block[dimension];b++){

	int ocb=1<<rhs._grid->CheckerBoardFromOindex(o+b);// Could easily be a table lookup
	if ( ocb &cbmask ) {
	  buffer[bo]=rhs._odata[so+o+b];
	  bo++;
	}

      }
      o +=rhs._grid->_slice_stride[dimension];
    }
  }
}

//////////////////////////////////////////////////////
// Gather for when there *is* need to SIMD split
//////////////////////////////////////////////////////
friend void Gather_plane_extract(Lattice<vobj> &rhs,std::vector<scalar_type *> pointers,int dimension,int plane,int cbmask)
{
  int rd = rhs._grid->_rdimensions[dimension];

  if ( !rhs._grid->CheckerBoarded(dimension) ) {

    int so  = plane*rhs._grid->_ostride[dimension]; // base offset for start of plane 
    int o   = 0;                                    // relative offset to base within plane
    int bo  = 0;                                    // offset in buffer

    // Simple block stride gather of SIMD objects
#pragma omp parallel for collapse(2)
    for(int n=0;n<rhs._grid->_slice_nblock[dimension];n++){
      for(int b=0;b<rhs._grid->_slice_block[dimension];b++){
	extract(rhs._odata[so+o+b],pointers);
      }
      o +=rhs._grid->_slice_stride[dimension];
    }

  } else { 

    int so  = plane*rhs._grid->_ostride[dimension]; // base offset for start of plane 
    int o   = 0;                                      // relative offset to base within plane
    int bo  = 0;                                      // offset in buffer
    
#pragma omp parallel for collapse(2)
    for(int n=0;n<rhs._grid->_slice_nblock[dimension];n++){
      for(int b=0;b<rhs._grid->_slice_block[dimension];b++){

	int ocb=1<<rhs._grid->CheckerBoardFromOindex(o+b);
	if ( ocb & cbmask ) {
	  extract(rhs._odata[so+o+b],pointers);
	}

      }
      o +=rhs._grid->_slice_stride[dimension];
    }
  }
}

//////////////////////////////////////////////////////
// Scatter for when there is no need to SIMD split
//////////////////////////////////////////////////////
friend void Scatter_plane_simple (Lattice<vobj> &rhs,std::vector<vobj,alignedAllocator<vobj> > &buffer,             int dimension,int plane,int cbmask)
{
  int rd = rhs._grid->_rdimensions[dimension];

  if ( !rhs._grid->CheckerBoarded(dimension) ) {

    int so  = plane*rhs._grid->_ostride[dimension]; // base offset for start of plane 
    int o   = 0;                                    // relative offset to base within plane
    int bo  = 0;                                    // offset in buffer

    // Simple block stride gather of SIMD objects
#pragma omp parallel for collapse(2)
    for(int n=0;n<rhs._grid->_slice_nblock[dimension];n++){
      for(int b=0;b<rhs._grid->_slice_block[dimension];b++){
	rhs._odata[so+o+b]=buffer[bo++];
      }
      o +=rhs._grid->_slice_stride[dimension];
    }

  } else { 

    int so  = plane*rhs._grid->_ostride[dimension]; // base offset for start of plane 
    int o   = 0;                                      // relative offset to base within plane
    int bo  = 0;                                      // offset in buffer
    
#pragma omp parallel for collapse(2)
    for(int n=0;n<rhs._grid->_slice_nblock[dimension];n++){
      for(int b=0;b<rhs._grid->_slice_block[dimension];b++){

	int ocb=1<<rhs._grid->CheckerBoardFromOindex(o+b);// Could easily be a table lookup
	if ( ocb & cbmask ) {
	  rhs._odata[so+o+b]=buffer[bo++];
	}

      }
      o +=rhs._grid->_slice_stride[dimension];
    }
  }
}

//////////////////////////////////////////////////////
// Scatter for when there *is* need to SIMD split
//////////////////////////////////////////////////////
friend void Scatter_plane_merge(Lattice<vobj> &rhs,std::vector<scalar_type *> pointers,int dimension,int plane,int cbmask)
{
  int rd = rhs._grid->_rdimensions[dimension];

  if ( !rhs._grid->CheckerBoarded(dimension) ) {

    int so  = plane*rhs._grid->_ostride[dimension]; // base offset for start of plane 
    int o   = 0;                                    // relative offset to base within plane
    int bo  = 0;                                    // offset in buffer

    // Simple block stride gather of SIMD objects
#pragma omp parallel for collapse(2)
    for(int n=0;n<rhs._grid->_slice_nblock[dimension];n++){
      for(int b=0;b<rhs._grid->_slice_block[dimension];b++){
	merge(rhs._odata[so+o+b],pointers);
      }
      o +=rhs._grid->_slice_stride[dimension];
    }

  } else { 

    int so  = plane*rhs._grid->_ostride[dimension]; // base offset for start of plane 
    int o   = 0;                                      // relative offset to base within plane
    int bo  = 0;                                      // offset in buffer
    
#pragma omp parallel for collapse(2)
    for(int n=0;n<rhs._grid->_slice_nblock[dimension];n++){
      for(int b=0;b<rhs._grid->_slice_block[dimension];b++){

	int ocb=1<<rhs._grid->CheckerBoardFromOindex(o+b);
	if ( ocb&cbmask ) {
	  merge(rhs._odata[so+o+b],pointers);
	}

      }
      o +=rhs._grid->_slice_stride[dimension];
    }
  }
}

//////////////////////////////////////////////////////
// local to node block strided copies
//////////////////////////////////////////////////////
friend void Copy_plane(Lattice<vobj>& lhs,Lattice<vobj> &rhs, int dimension,int lplane,int rplane,int cbmask)
{
  int rd = rhs._grid->_rdimensions[dimension];

  if ( !rhs._grid->CheckerBoarded(dimension) ) {

    int o   = 0;                                     // relative offset to base within plane
    int ro  = rplane*rhs._grid->_ostride[dimension]; // base offset for start of plane 
    int lo  = lplane*lhs._grid->_ostride[dimension]; // offset in buffer

  // Simple block stride gather of SIMD objects
#pragma omp parallel for collapse(2)
    for(int n=0;n<rhs._grid->_slice_nblock[dimension];n++){
      for(int b=0;b<rhs._grid->_slice_block[dimension];b++){
	lhs._odata[lo+o+b]=rhs._odata[ro+o+b];
      }
      o +=rhs._grid->_slice_stride[dimension];
    }

  } else {

    int ro  = rplane*rhs._grid->_ostride[dimension]; // base offset for start of plane 
    int lo  = lplane*lhs._grid->_ostride[dimension]; // base offset for start of plane 
    int o   = 0;                                     // relative offset to base within plane

#pragma omp parallel for collapse(2)
    for(int n=0;n<rhs._grid->_slice_nblock[dimension];n++){
      for(int b=0;b<rhs._grid->_slice_block[dimension];b++){

	int ocb=1<<lhs._grid->CheckerBoardFromOindex(o+b);

	if ( ocb&cbmask ) {
	  lhs._odata[lo+o+b]=rhs._odata[ro+o+b];
	}

      }
      o +=rhs._grid->_slice_stride[dimension];
    }

  }
}

friend void Copy_plane_permute(Lattice<vobj>& lhs,Lattice<vobj> &rhs, int dimension,int lplane,int rplane,int cbmask,int permute_type)
{
  int rd = rhs._grid->_rdimensions[dimension];


  if ( !rhs._grid->CheckerBoarded(dimension) ) {

    int o   = 0;                                     // relative offset to base within plane
    int ro  = rplane*rhs._grid->_ostride[dimension]; // base offset for start of plane 
    int lo  = lplane*rhs._grid->_ostride[dimension]; // offset in buffer

  // Simple block stride gather of SIMD objects
#pragma omp parallel for collapse(2)
    for(int n=0;n<rhs._grid->_slice_nblock[dimension];n++){
      for(int b=0;b<rhs._grid->_slice_block[dimension];b++){
	permute(lhs._odata[lo+o+b],rhs._odata[ro+o+b],permute_type);
      }
      o +=rhs._grid->_slice_stride[dimension];
    }

  } else {

    int ro  = rplane*rhs._grid->_ostride[dimension]; // base offset for start of plane 
    int lo  = lplane*lhs._grid->_ostride[dimension]; // base offset for start of plane 
    int o   = 0;                                     // relative offset to base within plane
    
#pragma omp parallel for collapse(2)
    for(int n=0;n<rhs._grid->_slice_nblock[dimension];n++){
      for(int b=0;b<rhs._grid->_slice_block[dimension];b++){

	int ocb=1<<lhs._grid->CheckerBoardFromOindex(o+b);

	if ( ocb&cbmask ) {
	  permute(lhs._odata[lo+o+b],rhs._odata[ro+o+b],permute_type);
	}

      }
      o +=rhs._grid->_slice_stride[dimension];
    }

  }
}

//////////////////////////////////////////////////////
// Local to node Cshift
//////////////////////////////////////////////////////
friend void Cshift_local(Lattice<vobj>& ret,Lattice<vobj> &rhs,int dimension,int shift)
{
  int sshift[2];

  sshift[0] = rhs._grid->CheckerBoardShift(rhs.checkerboard,dimension,shift,0);
  sshift[1] = rhs._grid->CheckerBoardShift(rhs.checkerboard,dimension,shift,1);

  if ( sshift[0] == sshift[1] ) {
    Cshift_local(ret,rhs,dimension,shift,0x3);
  } else {
    Cshift_local(ret,rhs,dimension,shift,0x1);// if checkerboard is unfavourable take two passes
    Cshift_local(ret,rhs,dimension,shift,0x2);// both with block stride loop iteration
  }
}

friend Lattice<vobj> Cshift_local(Lattice<vobj> &ret,Lattice<vobj> &rhs,int dimension,int shift,int cbmask)
{
  GridBase *grid = rhs._grid;
  int fd = grid->_fdimensions[dimension];
  int rd = grid->_rdimensions[dimension];
  int ld = grid->_ldimensions[dimension];
  int gd = grid->_gdimensions[dimension];

  // Map to always positive shift modulo global full dimension.
  shift = (shift+fd)%fd;

  ret.checkerboard = grid->CheckerBoardDestination(rhs.checkerboard,shift);
        
  // the permute type
  int permute_dim =grid->PermuteDim(dimension);
  int permute_type=grid->PermuteType(dimension);

  for(int x=0;x<rd;x++){       

    int o   = 0;
    int bo  = x * grid->_ostride[dimension];
    
    int cb= (cbmask==0x2)? 1 : 0;

    int sshift = grid->CheckerBoardShift(rhs.checkerboard,dimension,shift,cb);
    int sx     = (x+sshift)%rd;
	
    int permute_slice=0;
    if(permute_dim){
      int wrap = sshift/rd;
      int  num = sshift%rd;
      if ( x< rd-num ) permute_slice=wrap;
      else permute_slice = 1-wrap;
    }

    if ( permute_slice ) Copy_plane_permute(ret,rhs,dimension,x,sx,cbmask,permute_type);
    else                 Copy_plane(ret,rhs,dimension,x,sx,cbmask); 

  
  }
  return ret;
}

#endif
