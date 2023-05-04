#!/usr/bin/env python3.8

import sys
import matplotlib.pyplot as plt
import configparser

import hashfilereader

# Hold configuration information
config_parser = None

def generate_shared_chunk_histogram_data(hash_reader: hashfilereader.HashFileReader) -> list:
    """
        Creates the underlying data for a chunk sharing histogram
        @param hash_reader: Instance of hashfilereader.HashFileReader which has properties of all desired chunks stored.
        @return: List containing counts of shared chunks. Each index has the number of chunks shared across <index_value> clients.
                Eg: Index 1 has the count of chunks owned by only 1 client
                    Index 2 has the count of chunks owned by 2 clients.
    """

    chunk_properties = hash_reader.get_chunk_properties()
    num_hash_files = hash_reader.get_files_read()

    # List to hold counts of shared chunks
    shared_chunk_counts = [0 for i in range(num_hash_files+1)]
    # Remove value at first index (always 0 as it represents the chunks shared by 0 clients)
    shared_chunk_counts.pop(0)
    
    # Iterate over global dict of records
    for chunk_hash, chunk_props in chunk_properties.items():
        chunk_client_count = len(chunk_props['files'])
        shared_chunk_counts[chunk_client_count] += 1

    hist_x = [i+1 for i in range(hash_reader.get_files_read())]


    return shared_chunk_counts, hist_x

def plot_histogram(hist_x, hist_y):
    """
        Function to plot histogram along with relevant styling code
    """
    
    analysis_mode = config_parser['OPERATION']['AnalysisMode']
    if(analysis_mode == "chunk-sharing"):
        fig_save_path = config_parser['CHUNKSHARING']['HistogramFigureName']
    elif(analysis_mode == "chunk-frequency"):
        fig_save_path = config_parser['CHUNKFREQUENCY']['HistogramFigureName']


    plt.style.use('ggplot')
    
    plt.bar(hist_x, hist_y)
    plt.xticks(hist_x)
    
    plt.tight_layout()
    plt.savefig(fig_save_path)


if(__name__ == "__main__"):
    # Main driver code to plot histograms
    if(len(sys.argv) < 2):
        print("Usage: ./generate_chunk_frequency_breakdown.py <config_file> <client_hash_file_1> [<client_hash_file_2>...]")     

    config_path = sys.argv[1]
    client_hash_file_paths = [sys.argv[i] for i in range(2, len(sys.argv))]
    
    config_parser = configparser.ConfigParser()
    config_parser.read(config_path)

    hash_reader = hashfilereader.HashFileReader()

    # Read all input hash files
    hash_reader.read_hash_files(client_hash_file_paths)

    # Calculate shared chunk counts
    hist_y, hist_x = generate_shared_chunk_histogram_data(hash_reader) 
    
    # Plot and save
    plot_histogram(hist_x, hist_y)
    
