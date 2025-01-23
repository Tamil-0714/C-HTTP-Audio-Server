#include "bstfnc.h"
#include "filehash.h"
#include <stdio.h>

int main() {
  files_hash =
      initFiles("NULL_KEY", "NULL_VALUE"); // Initialize the global variable
  addFiles(files_hash, "nsf3", "3efs");
  addFiles(files_hash, "nsf5", "5efs");
  addFiles(files_hash, "nsf2", "2efs");
  inorder(files_hash);

  return 0;
}