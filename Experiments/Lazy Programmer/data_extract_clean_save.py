#!/usr/bin/env python

import sys, getopt
import argparse
import numpy as np
import pandas as pd

def my_print():
    print("hey!");

def get_data(in_file_path, in_column_names):
    print(in_file_path);
    print(in_column_names);

    original_data = pd.read_csv(in_file_path)
    
    data = np.array([[]]);
    
    column_list = list(in_column_names.split(" "));
    
    for column in original_data.columns: 
        if column in column_list:
            column_entries = original_data[column].tolist();
            print(type(column_entries));
            print(column_entries[67]);
            np.add(data, column_entries);

def main():#argv):
    parser = argparse.ArgumentParser(description='Handle input data.');
    default_data_file = 'C:/Users/alime/Downloads/netflix-shows/netflix_titles_nov_2019.csv';
    parser.add_argument('--DATA_FILE_PATH', nargs='?', default=default_data_file);
    parser.add_argument('--COLUMN_NAMES', nargs='?');
    args = parser.parse_args();
   
    get_data(args.DATA_FILE_PATH, args.COLUMN_NAMES);

if __name__ == "__main__":
   main()#sys.argv[1:])