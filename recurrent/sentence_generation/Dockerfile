FROM python:latest

RUN pip install tensorflow
RUN pip install spacy
RUN pip install numpy keras
RUN pip install grammar
RUN pip install music21
RUN pip install IPython
# preprocess data_utils qa

COPY ./ ./

CMD python main.py
