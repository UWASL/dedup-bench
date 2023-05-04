class HashFileReader:
    """
        Class to read hash files and store chunk properties indexed by chunk hash
    """
    def __init__(self) -> None:
        """
            Default Constructor
        """

        # Dictionary holding properties of all read chunks. Indexed by chunk_hash from the hash files.
            # Each entry is itself a dictionary holding all the properties of the relevant chunk.
        self.chunk_properties = {}
        
        # Number of files read so far
        self.hash_files_read = 0
        pass

    def _create_chunk_record(self, chunk_hash: str, chunk_size: int) -> dict:    
        """
            Create a dictionary entry for chunk with hash 'chunk_hash' and initialize it.
            @param chunk_hash: Hash of new chunk
            @return: Pointer to newly created entry for ease of access
        """

        self.chunk_properties[chunk_hash] = {}
        
        new_entry = self.chunk_properties[chunk_hash]
        new_entry['size'] = chunk_size
        new_entry['files'] = []
        new_entry['frequency'] = 0

        return new_entry

    def get_chunk_properties(self) -> dict:
        """
            Return a pointer to stored chunk properties
        """
        return self.chunk_properties

    def get_files_read(self) -> int:
        """
            Return number of hash files read so far
        """
        return self.hash_files_read

    def read_hash_file(self, hash_file_path: str) -> None:
        """
            Read a single hash file and store all chunk properties
            @param hash_file_path: Path to hash file
            @return: None
        """

        chunk_properties = self.chunk_properties
        self.hash_files_read += 1

        with open(hash_file_path, 'r') as hash_file:
            for line in hash_file:
                line_split = line.strip().split(",")

                if(len(line_split)) != 2:
                    continue

                chunk_hash = line_split[0].strip()
                chunk_size = int(line_split[1].strip())

                if(chunk_hash not in chunk_properties.keys()):
                    # New chunk hash observed
                    chunk_entry = self._create_chunk_record(chunk_hash, chunk_size)
                    chunk_entry['files'].append(self.hash_files_read)
                    chunk_entry['frequency'] = 1
                else:
                    # Update record for existing chunk hash
                    chunk_entry = chunk_properties[chunk_hash]

                    # Sanity check to make sure the same hash isn't present with 2 different chunk sizes
                    if(chunk_entry['size'] != chunk_size):
                        raise Exception('Chunk ' + str(chunk_hash) + 'has different sizes: ' + str(chunk_size) + "," + str(chunk_entry['size']))
                    
                    # New client holding this chunk
                    if(self.hash_files_read not in chunk_entry['clients']):
                        chunk_entry['files'].append(self.hash_files_read)

                    # New occurence of chunk
                    chunk_entry['frequency'] += 1
    
    def read_hash_files(self, hash_files: list) -> None:
        """
            Read all specified hash files and store all chunk properties
            @param hash_files: List of str containing paths to each hash file
            @return: None
        """

        # Iterate over all specified files
        for file_path in hash_files:
                self.read_hash_file(file_path)



