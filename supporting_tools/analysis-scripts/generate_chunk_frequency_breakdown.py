#!/usr/bin/env python3.8

import sys
import matplotlib.pyplot as plt

"""
    Dictionary holding properties of all read chunks. Indexed by chunk_hash from the hash files.
    Each entry is itself a dictionary holding all the properties of the relevant chunk.
"""
CHUNK_PROPERTIES = {}
NUM_CLIENTS = 0
FIG_SAVE_PATH = 'chunk_sharing.jpg'


def create_chunk_record(chunk_hash: str, chunk_size: int) -> None:
    global CHUNK_PROPERTIES
    
    """
        Create a dictionary entry for chunk with hash 'chunk_hash' and initialize it.
        @param chunk_hash: Hash of new chunk
        @return: Pointer to newly created entry for ease of access
    """

    CHUNK_PROPERTIES[chunk_hash] = {}
    
    new_entry = CHUNK_PROPERTIES[chunk_hash]
    new_entry['size'] = chunk_size
    new_entry['clients'] = []
    new_entry['frequency'] = 0

    return new_entry


def read_hash_files(hash_file_paths) -> None:
    global CHUNK_PROPERTIES, NUM_CLIENTS
    """
        Iterate over all specified hash files and calculate chunk ownership counts
        @param hash_file_paths: List of hash files to read. Function assumes one hash file per client.
        @return: None
    """

    NUM_CLIENTS = len(hash_file_paths)

    # Iterate over each hash file
    for i in range(NUM_CLIENTS):
        with open(hash_file_paths[i], 'r') as hash_file:
            for line in hash_file:
                line_split = line.strip().split(",")

                if(len(line_split)) != 2:
                    continue

                chunk_hash = line_split[0].strip()
                chunk_size = int(line_split[1].strip())

                if(chunk_hash not in CHUNK_PROPERTIES.keys()):
                    # New chunk hash observed
                    chunk_entry = create_chunk_record(chunk_hash, chunk_size)
                    chunk_entry['clients'].append(i)
                    chunk_entry['frequency'] = 1
                else:
                    # Update record for existing chunk hash
                    chunk_entry = CHUNK_PROPERTIES[chunk_hash]

                    # Sanity check to make sure the same hash isn't present with 2 different chunk sizes
                    if(chunk_entry['size'] != chunk_size):
                        raise Exception('Chunk ' + str(chunk_hash) + 'has different sizes: ' + str(chunk_size) + "," + str(chunk_entry['size']))
                    
                    # New client holding this chunk
                    if(i not in chunk_entry['clients']):
                        chunk_entry['clients'].append(i)

                    # New occurence of chunk
                    chunk_entry['frequency'] += 1

def generate_chunk_histogram_data() -> list:
    """
        Creates the underlying data for a chunk sharing histogram
        @param: None
        @return: List containing counts of shared chunks. Each index has the number of chunks shared across <index_value> clients.
                Eg: Index 1 has the count of chunks owned by only 1 client
                    Index 2 has the count of chunks owned by 2 clients.
    """
    global CHUNK_PROPERTIES

    # List to hold counts of shared chunks
    shared_chunk_counts = [0 for i in range(NUM_CLIENTS+1)]

    # Iterate over global dict of records
    for chunk_hash, chunk_props in CHUNK_PROPERTIES.items():
        chunk_client_count = len(chunk_props['clients'])
        shared_chunk_counts[chunk_client_count] += 1

    return shared_chunk_counts

def plot_histogram(hist_x, hist_y):
    """
        Function to plot histogram along with relevant styling code
    """
    
    plt.style.use('ggplot')
    
    plt.bar(hist_x, hist_y)
    plt.xticks(hist_x)
    
    plt.tight_layout()
    plt.savefig(FIG_SAVE_PATH)


if(__name__ == "__main__"):
    if(len(sys.argv) < 2):
        print("Usage: ./generate_chunk_frequency_breakdown.py <client_hash_file_1> [<client_hash_file_2>...]")     

    client_hash_file_paths = [sys.argv[i] for i in range(1, len(sys.argv))]
    
    # Read all input hash files
    read_hash_files(client_hash_file_paths)

    # Calculate shared chunk counts
    hist_y = generate_chunk_histogram_data()
    # Remove value at first index (always 0 as it represents the chunks shared by 0 clients)
    hist_y.pop(0)
    
    hist_x = [i+1 for i in range(NUM_CLIENTS)]

    # Plot and save
    plot_histogram(hist_x, hist_y)
    


    
            
            
        