/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : pixel_map_designer
 * @created     : Thursday May 02, 2024 10:57:39 CEST
 */

#include <iostream>
#include <fstream>
#include <TH2Poly.h>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include <rapidjson/filewritestream.h>

const char* unit = "mm";

rapidjson::Value add_point(const double z, const double x, rapidjson::Document& d) {
  rapidjson::Value jj(rapidjson::kObjectType); 
  rapidjson::Value jz(rapidjson::kObjectType); 
  jz.AddMember("val", z, d.GetAllocator()); 
  jz.AddMember("unit", rapidjson::StringRef(unit), d.GetAllocator()); 
  jj.AddMember("z", jz, d.GetAllocator()); 
  rapidjson::Value jx(rapidjson::kObjectType);       
  jx.AddMember("val", x, d.GetAllocator()); 
  jx.AddMember("unit", rapidjson::StringRef(unit), d.GetAllocator()); 
  jj.AddMember("x", jx, d.GetAllocator()); 

  return jj;
}

int pixel_map_designer()
{
  const double cell_x_dim = 32; 
  const double cell_z_dim = 32; 

  const double pix_z_dim = 4;
  const double pix_x_dim = 4;

  const int n_z = std::floor( cell_z_dim / pix_z_dim );
  const int n_x = std::floor( cell_x_dim / pix_x_dim );

  FILE* fp = fopen("output.json", "w");
  char writeBuffer[65536];
  rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
  rapidjson::StringBuffer buffer;
 
  rapidjson::Document d; 
  d.SetObject(); 
  rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);

  rapidjson::Value jpixelmap(rapidjson::kArrayType);
  
  UInt_t pix_nr = 1; 
  for (int iz = 0; iz < n_z; iz++) {
    for (int ix = 0; ix < n_x; ix++) {

      const double _z = (-0.5*(n_z-1) + iz)*pix_z_dim;
      const double _x = (-0.5*(n_x-1) + ix)*pix_x_dim;

      rapidjson::Value jpix(rapidjson::kObjectType); 
      char name_c[10]; std::sprintf(name_c, "qpix%u", pix_nr); 
      printf("pixel name: %s\n", name_c); 
      jpix.AddMember("name", rapidjson::StringRef(name_c), d.GetAllocator()); 

      rapidjson::Value jedges(rapidjson::kArrayType); 
      rapidjson::Value j0 = add_point(_z-0.5*pix_z_dim, _x-0.5*pix_x_dim, d); jedges.PushBack(j0, d.GetAllocator()); 
      rapidjson::Value j1 = add_point(_z+0.5*pix_z_dim, _x-0.5*pix_x_dim, d); jedges.PushBack(j1, d.GetAllocator()); 
      rapidjson::Value j2 = add_point(_z+0.5*pix_z_dim, _x+0.5*pix_x_dim, d); jedges.PushBack(j2, d.GetAllocator()); 
      rapidjson::Value j3 = add_point(_z-0.5*pix_z_dim, _x+0.5*pix_x_dim, d); jedges.PushBack(j3, d.GetAllocator()); 
      rapidjson::Value j4 = add_point(_z-0.5*pix_z_dim, _x-0.5*pix_x_dim, d); jedges.PushBack(j4, d.GetAllocator()); 

      jpix.AddMember("edges", jedges, d.GetAllocator()); 

      jpixelmap.PushBack( jpix, d.GetAllocator() ); 
      pix_nr++;
    }
  }

  d.AddMember("pixelmap", jpixelmap, d.GetAllocator()); 
  
  d.Accept( writer ); 
  fclose( fp ); 


  return 0;
}

