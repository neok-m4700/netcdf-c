/* This is part of the netCDF package.
   Copyright 2018 University Corporation for Atmospheric Research/Unidata
   See COPYRIGHT file for conditions of use.

   Test netcdf files a bit.

   Ed Hartnett
*/

#include <nc_tests.h>
#include "err_macros.h"
#include "hdf5internal.h"

#define FILE_NAME "tst_files6.nc"
#define HDF5_FILE_NAME "tst_files6.h5"
#define GROUP_NAME "Britany_Fans"
#define GROUP_NAME_2 "Toxic_Fans"
#define TRUE_FANS "True_Toxic_Fans"

int
main(int argc, char **argv)
{
   printf("\n*** Testing netcdf file functions some more.\n");
   printf("*** testing Jeff Whitaker's test...");
   {
#define DIM_NAME "xc"
#define DIM_LEN 134
#define VAR_NAME1 "var1"
#define VAR_NAME2 "var2"

      int ncid, dimid, varid1, varid2, dimid_in;
      int ndims_in, natts_in;
      size_t len_in;
      char name_in[NC_MAX_NAME + 1];
      nc_type xtype_in;

      if (nc_create(FILE_NAME, NC_CLOBBER|NC_NETCDF4, &ncid)) ERR;
      if (nc_def_dim(ncid, DIM_NAME, DIM_LEN, &dimid)) ERR;
      if (nc_def_var(ncid, VAR_NAME1, NC_FLOAT, 1, &dimid, &varid1)) ERR;
      if (nc_def_var(ncid, VAR_NAME2, NC_FLOAT, 1, &dimid, &varid2)) ERR;
      if (nc_def_var(ncid, DIM_NAME, NC_FLOAT, 1, &dimid, &varid2)) ERR;
      if (nc_close(ncid)) ERR;

      /* Open and check. */
      if (nc_open(FILE_NAME, NC_CLOBBER|NC_NETCDF4, &ncid)) ERR;
      if (nc_inq_dim(ncid, 0, name_in, &len_in)) ERR;
      if (strcmp(name_in, DIM_NAME) || len_in != DIM_LEN) ERR;
      if (nc_inq_var(ncid, 0, name_in, &xtype_in, &ndims_in,
                     &dimid_in, &natts_in)) ERR;
      if (strcmp(name_in, VAR_NAME1) || xtype_in != NC_FLOAT ||
          ndims_in != 1 || dimid_in != 0 || natts_in != 0) ERR;
      if (nc_inq_var(ncid, 1, name_in, &xtype_in, &ndims_in,
                     &dimid_in, &natts_in)) ERR;
      if (strcmp(name_in, VAR_NAME2) || xtype_in != NC_FLOAT ||
          ndims_in != 1 || dimid_in != 0 || natts_in != 0) ERR;
      if (nc_inq_var(ncid, 2, name_in, &xtype_in, &ndims_in,
                     &dimid_in, &natts_in)) ERR;
      if (strcmp(name_in, DIM_NAME) || xtype_in != NC_FLOAT ||
          ndims_in != 1 || dimid_in != 0 || natts_in != 0) ERR;
      if (nc_close(ncid)) ERR;
   }
   SUMMARIZE_ERR;
   printf("*** testing HDF5 file with circular group structure...");
   {
      hid_t hdfid, grpid, grpid2;
      int ncid;

      /* First use HDF5 to create a file with circular group
       * struct. */
      if ((hdfid = H5Fcreate(HDF5_FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT,
                             H5P_DEFAULT)) < 0) ERR;
      if ((grpid = H5Gcreate2(hdfid, GROUP_NAME, H5P_DEFAULT, H5P_DEFAULT,
                              H5P_DEFAULT)) < 0) ERR;
      if ((grpid2 = H5Gcreate2(grpid, GROUP_NAME_2, H5P_DEFAULT, H5P_DEFAULT,
                               H5P_DEFAULT)) < 0) ERR;
      if (H5Lcreate_soft(GROUP_NAME, grpid2, TRUE_FANS, H5P_DEFAULT,
                         H5P_DEFAULT) < 0) ERR;
      if (H5Fclose(hdfid) < 0) ERR;

      /* Force HDF5 to forget about this file. This resets the error
       * stack, so turn off HDF5 error messages again. */
      H5close();
      H5Eset_auto2(H5E_DEFAULT, NULL, NULL);

      /* Now try and open it with netCDF. It will not work. */
      if (nc_open(HDF5_FILE_NAME, NC_NOWRITE, &ncid) != NC_EHDFERR) ERR;
   }
   SUMMARIZE_ERR;
   FINAL_RESULTS;
}
