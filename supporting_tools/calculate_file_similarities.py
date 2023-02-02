#!/usr/bin/env python3
import os, sys

# Number of bytes to read and compare in an interation i.e. read block size
BLOCK_SIZE = 4096

def similar_char_count(string_1: str, string_2: str) -> int:
    """
    Returns the number of similar (same characters at same positions) characters between two given strings
    @param string_1: First input string
    @param string_2: Second input string 
    """
    return(sum(string_1[i] == string_2[i] for i in range(min(len(string_1), len(string_2)))))


def calc_similarity(filename_1: str, filename_2: str) -> float:
    """
    Calculates the degree of similarity between 2 files (percentage of similar characters across the files)
    @param filename_1: Path to file 1
    @param filename_2: Path to file 2
    """
    file_1_size = 0
    file_2_size = 0

    # Check if both files exist
    if(os.path.exists(filename_1) == False):
        print(filename_1,"does not exist")
        sys.exit(1)
    
    if(os.path.exists(filename_2) == False):
        print(filename_2,"does not exist")
        sys.exit(1)

    # Get file sizes in bytes
    file_1_size = (os.stat(filename_1).st_size)
    file_2_size = (os.stat(filename_2).st_size)
   
    # Handle unequal file sizes
    if(file_1_size != file_2_size):
        print("File sizes aren't equal.")
        print("\t",filename_1,":",file_1_size/1024,"Kb")
        print("\t",filename_2,":",file_2_size/1024,"Kb")
    
        print("Using larger size: ", max(file_1_size, file_2_size), "for similarity percentages")
    

    # Calculate similar characters
    total_similar_chars = 0
    with open(filename_1, 'r') as file_1:
        with open(filename_2, 'r') as file_2:
            bytes_read = 0
            size_limit = min(file_1_size, file_2_size) 

            # Control number of bytes to read in next iteration
            bytes_to_read_next = BLOCK_SIZE
    
            while(bytes_read < size_limit):
                # Read a block from both files
                bytes_1 = file_1.read(bytes_to_read_next)
                bytes_2 = file_2.read(bytes_to_read_next)

                # Calculate similar characters in blocks
                total_similar_chars += similar_char_count(bytes_1, bytes_2)

                # Set block size for next read
                bytes_read += bytes_to_read_next
                bytes_to_read_next = (min((size_limit - bytes_read), BLOCK_SIZE))               
    
    return ((total_similar_chars) / max(file_1_size, file_2_size))


if(__name__ == "__main__"):
    if(len(sys.argv) != 3):
        print("Usage: ./calculate_file_similarities.py <file_1_path> <file_2_path>")
        sys.exit(1)
    
    filename_1 = sys.argv[1]
    filename_2 = sys.argv[2]

    similarity_degree = calc_similarity(filename_1, filename_2)

    print("Similarity Degree: ", similarity_degree)

