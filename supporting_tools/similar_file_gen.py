#!/usr/bin/env python3
import sys, random, string

# Percentage of similirty between both files on average
SIMILARITY_DEGREE = 0.6


# Using ascii and newline for now
char_set = string.ascii_letters + string.digits + "\n"
# char_set = string.printable


def file_creator(filename_1: str, filename_2: str, file_size_bytes: int) -> None:
    """
        Creates 2 files with their degree of similarit equal to SIMILARITY_DEGREE
        @param filename_1: Name of file 1
        @param filename_2: Name of file 2
        @param file_size_bytes: Size of files in bytes
    """
    with open(filename_1, 'w') as file_1:
        with open(filename_2, 'w') as file_2:
            
            # Seed random with sys time
            random.seed()
            for i in range(file_size_bytes):
                similar_check = random.random()
                random_char_1 = random.sample(char_set, 1)[0]
                
                if(similar_check <= SIMILARITY_DEGREE):
                    # Write same character to both files
                    file_1.write(random_char_1)
                    file_2.write(random_char_1)
                else:
                    # Generate another character for file 2
                    random_char_2 = random_char_1
                    while(random_char_2 == random_char_1):
                        random_char_2 = random.sample(char_set, 1)[0]
                    
                    file_1.write(random_char_1)
                    file_2.write(random_char_2)

    print("Files:", filename_1,"and",filename_2,"have been created")

if(__name__ == "__main__"):
    if(len(sys.argv) != 5):
        print("Usage: ./similar_file_gen.py <file_name_1> <file_name_2> <similarity_degree> <file_size>")
        print("\t<similarity_degree> - Average percentage of similarity between the files (0.0 - 1.0)")
        print("\t<file_size> - Size of both files in kilobytes")
        sys.exit(0)
    
    filename_1 = sys.argv[1]
    filename_2 = sys.argv[2]
    try:
        SIMILARITY_DEGREE = float(sys.argv[3])
    except ValueError:
        print("<similarity_degree> needs to be in the range [0.0,1.0]")
        sys.exit(0)
    
    try:
        file_size = int(sys.argv[4])
    except ValueError:
        print("<file_size> needs to be an integer")
        sys.exit(0)

    file_creator(filename_1, filename_2, file_size * 1000)
    
                    


