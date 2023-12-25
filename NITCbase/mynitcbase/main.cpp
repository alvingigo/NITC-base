#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include <iostream>
#include <cstring>
using namespace std;
// int main(int argc, char *argv[]) {
//   Disk disk_run;

//   unsigned char buffer[BLOCK_SIZE];
//   Disk::readBlock(buffer, 7000);
//   char message[] = "hello";
//   memcpy(buffer + 20, message, 6);
//   Disk::writeBlock(buffer, 7000);

//   unsigned char buffer2[BLOCK_SIZE];
//   char message2[6];
//   Disk::readBlock(buffer2, 7000);
//   memcpy(message2, buffer2 + 20, 6);
//   std::cout << message2;

//   unsigned char buffer3[BLOCK_SIZE];
//   Disk::readBlock(buffer3,1);
//   char message3[1];
//   memcpy(message3,buffer3+20,1);
//   std::cout<<message3;
  


//   // unsigned char buffer3[BLOCK_SIZE];
//   // int j;
//   // for (int i=0;i<6;i++){
//   //   Disk::readBlock(buffer3,i);
//   //   memcpy(j,buffer3,4);
//   //   cout<<j<<endl;
//   //}



//   // StaticBuffer buffer;
//   // OpenRelTable cache;
//   return 0;
//   /*return FrontendInterface::handleFrontend(argc, argv);*/
// }
//stage 2
// int main(int argc, char *argv[]) {
//   Disk disk_run;
//   StaticBuffer buffer;
//   OpenRelTable cache;
//   // create objects for the relation catalog and attribute catalog
//   RecBuffer relCatBuffer(RELCAT_BLOCK);//4
//   RecBuffer attrCatBuffer(ATTRCAT_BLOCK);//5
//   RecBuffer attrCatBuffer1(ATTRCAT_BLOCK+1);//6

//   HeadInfo relCatHeader;
//   HeadInfo attrCatHeader;
//   HeadInfo attrCatHeader1;

//   // load the headers of both the blocks into relCatHeader and attrCatHeader.
//   // (we will implement these functions later)
//   relCatBuffer.getHeader(&relCatHeader);
//   attrCatBuffer.getHeader(&attrCatHeader);
//   attrCatBuffer1.getHeader(&attrCatHeader1);

//   pair<int,int> bb;
//   for (int i=0;i<relCatHeader.numEntries;i++) { //num of records

//     Attribute relCatRecord[RELCAT_NO_ATTRS]; // will store the record from the relation catalog
   
//     relCatBuffer.getRecord(relCatRecord, i);

//     printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);

//     for (int j=0;j<attrCatHeader.numSlots;j++) {//change to numslots

//       // declare attrCatRecord and load the attribute catalog entry into it
//       Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
//       attrCatBuffer.getRecord(attrCatRecord,j);






//       if (!strcmp (attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,relCatRecord[RELCAT_REL_NAME_INDEX].sVal)) {/* attribute catalog entry corresponds to the current relation */
//         const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
        
//         if(!strcmp(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,"Batch")){
//           bb={j,ATTRCAT_BLOCK};
//           printf("Kundi\n");
//         }
//              printf("  %d: %s\n",attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal, attrType);
//         // printf("  %s",attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal);//, attrType);
//       }
//     }
//       // cout<<attrCatHeader1.numEntries;
//       for(int k=0;  k<attrCatHeader1.numEntries; k++){
//         Attribute attrCatRecord1[ATTRCAT_NO_ATTRS];
//         attrCatBuffer1.getRecord(attrCatRecord1,k);//  STG 2 exercise
//         if (!strcmp (attrCatRecord1[ATTRCAT_REL_NAME_INDEX].sVal,relCatRecord[RELCAT_REL_NAME_INDEX].sVal)) {/* attribute catalog entry corresponds to the current relation */
//           const char *attrType = attrCatRecord1[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
//           if(attrCatRecord1[ATTRCAT_ATTR_NAME_INDEX].sVal=="Class"){
//           bb={k,ATTRCAT_BLOCK+1};
//           cout<<"kundi";
//         }
//           printf("  %d: %s\n",attrCatRecord1[ATTRCAT_ATTR_TYPE_INDEX].nVal, attrType);
//           // printf("  %s",attrCatRecord1[ATTRCAT_ATTR_NAME_INDEX].sVal);//, attrType);
        
//       }
    
//     printf("\n");
//   }


//   unsigned char buff[BLOCK_SIZE];
//   Disk::readBlock(buff,bb.second);
//   unsigned char* ptr = &buff[HEADER_SIZE+SLOTMAP_SIZE_RELCAT_ATTRCAT+bb.first*ATTR_SIZE*ATTRCAT_NO_ATTRS+ATTR_SIZE];
//   memcpy(ptr,"Class",6);
//   Disk::writeBlock(buff,bb.second);
//   cout<<"bb";
//     for (int i=0;i<relCatHeader.numEntries;i++) { //num of records

//     Attribute relCatRecord[RELCAT_NO_ATTRS]; // will store the record from the relation catalog
   
//     relCatBuffer.getRecord(relCatRecord, i);

//     printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);

//     for (int j=0;j<attrCatHeader.numSlots;j++) {//change to numslots

//       // declare attrCatRecord and load the attribute catalog entry into it
//       Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
//       attrCatBuffer.getRecord(attrCatRecord,j);






//       if (!strcmp (attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,relCatRecord[RELCAT_REL_NAME_INDEX].sVal)) {/* attribute catalog entry corresponds to the current relation */
//         const char *attrType = attrCatRecord[ATTRCAT_PRIMARY_FLAG_INDEX].nVal == NUMBER ? "NUM" : "STR";
//         if(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal=="Class"){
//           bb={j,ATTRCAT_BLOCK};
//         }
//         printf("  %d: %s\n",attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].nVal, attrType);
//       }
//     }
//       // cout<<attrCatHeader1.numEntries;
//       for(int k=0;  k<attrCatHeader1.numEntries; k++){
//         Attribute attrCatRecord1[ATTRCAT_NO_ATTRS];
//         attrCatBuffer1.getRecord(attrCatRecord1,k);//  STG 2 exercise
//         if (!strcmp (attrCatRecord1[ATTRCAT_REL_NAME_INDEX].sVal,relCatRecord[RELCAT_REL_NAME_INDEX].sVal)) {/* attribute catalog entry corresponds to the current relation */
//           const char *attrType = attrCatRecord1[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
//           if(attrCatRecord1[ATTRCAT_ATTR_NAME_INDEX].sVal=="Class"){
//           bb={k,ATTRCAT_BLOCK+1};
//         }
//           printf("  %d: %s\n",attrCatRecord1[ATTRCAT_ROOT_BLOCK_INDEX].nVal, attrType);
          
//         }
//       }
    
//     printf("\n");
//   }
//   return 0;

// }
// }

//stage 3

// int main(int argc, char *argv[]) {
//   Disk disk_run;
//   StaticBuffer buffer;
//   OpenRelTable cache;

  /*
  for i = 0 and i = 1 (i.e RELCAT_RELID and ATTRCAT_RELID)

      get the relation catalog entry using RelCacheTable::getRelCatEntry()
      printf("Relation: %s\n", relname);

      for j = 0 to numAttrs of the relation - 1
          get the attribute catalog entry for (rel-id i, attribute offset j)
           in attrCatEntry using AttrCacheTable::getAttrCatEntry()

          printf("  %s: %s\n", attrName, attrType);
  */
  // RelCacheTable RelCat;
  // AttrCacheTable AttCat;


  // RelCatEntry relentry;
  // AttrCatEntry attentry;

//  for (int i=RELCAT_RELID;i<=ATTRCAT_RELID;i++){
//   RelCat.getRelCatEntry(i,&relentry);
//   printf("Relation: %s\n", relentry.relName);
//   for(int j=0;j<relentry.numAttrs;j++){
//     AttrCacheTable::getAttrCatEntry(i,j,&attentry);
//     int num= attentry.attrType;
//     const char* attrType= num== NUMBER? "NUM": "STR";
//     printf("  %s: %s\n", attentry.attrName,attrType);
//   }



//  }

  // RelCat.getRelCatEntry(2,&relentry);
  // printf("Relation: %s\n", relentry.relName);
  // for(int j=0;j<relentry.numAttrs;j++){
  //   AttrCacheTable::getAttrCatEntry(2,j,&attentry);
  //   int num= attentry.attrType;
  //   const char* attrType= num== NUMBER? "NUM": "STR";
  //   printf("  %s: %s\n", attentry.attrName,attrType);
  // }


  //return FrontendInterface::handleFrontend(argc,argv);
  
//}

int main(int argc, char *argv[]) {
  Disk disk_run;
  StaticBuffer buffer;
  OpenRelTable cache;


  // RecBuffer rec(5);
  // Attribute record[6];
  // rec.getRecord(record,0);
  // printf("%d  ",record[4].nVal);
  return FrontendInterface::handleFrontend(argc,argv);
  
}