
import numpy as np
import pandas as pd

#colnames = ['year', 'name', 'city', 'latitude', 'longitude']
data = pd.read_csv('netflix_titles_nov_2019.csv')#, names=colnames)
#data = data.apply(pd.to_numeric, errors='coerce');

# TODO: Convert strings int categories (e.g. country of UK = 0, France = 1)

countries_as_strings = data.country.tolist();
countries_as_array = [];

for country in countries_as_strings:
    if country not in countries_as_array:
        countries_as_array.append(country);
        print("added ", country);

X = np.array([data.director.tolist()]);
Y = np.array([]);

#((X.T, X), (X.T, Y));