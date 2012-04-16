/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef FMI_ZIP_UNZIP_H_
#define FMI_ZIP_UNZIP_H_

#ifdef __cplusplus 
extern "C" {
#endif

#include <stdlib.h>
#include <jm_types.h>
#include "miniunz.h"
/**
 \file fmi_zip_unzip.h
 Declaration of fmi_zip_unzip() function.

 \addtogroup fmi_zip Interface to zlib
 @{
*/

/**
 * \brief Uncompress a zip file
 * 
 * @param zip_file_path Full file path of the file to uncompress.
 * @param output_folder Full file path of the directory where the uncompressed files are put. The folder must already exist. Files with the same name are overwritten.
 * @param callbacks Callback functions
 * @return Error status.
 */
jm_status_enu_t fmi_zip_unzip(const char* zip_file_path, const char* output_folder, jm_callbacks* callbacks);

/** @} */

#ifdef __cplusplus 
}
#endif

#endif /* End of header file FMI_ZIP_UNZIP_H_ */
