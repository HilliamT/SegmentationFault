# SegmentationFault

A command line interface browser tool built in C to search through StackOverflow's catalogue of answered questions. This was built for an open-ended group project task to develop a program in C.

![](https://i.imgur.com/5oKL046.png)

## Features
The command line interface browser offers a number of features to ensure that the experience is vibrant and the browser is accessible to use.

- Question Overviews
- Vim Keyboard Navigations
- Language-Specific Syntax Highlighting
- Typo Correction
- Deep Question View with Answers

## Demonstration

The project, built in C, will need to be built first before it can be ran.

    $ make

To run the program, ask a question:

    $ ./sf "My question is ..."

A list of questions will be shown pulled from StackOverflow that will be relevant to your query.

To expand a question, simply press `Enter` to read the full question and its best answer, alongside other answers.

![](https://i.imgur.com/YyATlYq.png)

### Help Screen

A menu of available commands is provided when using the tag `--help`.

    $ ./sf --help

## How It Works

The program is built around the StackOverflow public API endpoint that is used for its search functionality.

    GET api.stackexchange.com/2.2/search/advanced?order=desc&sort=relevance&accepted=True&answers=1&site=stackoverflow&filter=!-*jbN-o8P3E5&q=RESPONSE

This endpoint returns the most relevant questions first that have at least one accepted answer to them, which we query using the terminal `RESPONSE` given in from the user.

Official documentation for this endpoint can be viewed [here](https://api.stackexchange.com/docs/advanced-search).

### Defining the Pipeline

Our program will go through a series of steps to process the user's terminal input, query the API and display an overview of all the questions. The program will also display answered questions when a user chooses to expand on a question as an optional step. The full list of stages are:

1. User Input Processing
2. API Calling
3. API Response Handler
4. Displaying

### User Input Processing

The user's question query accepted as the first command line argument, unless the `--help` flag is given.

    $ ./sf "My question is ..."

The input is saved, passed onto the API calling methods.

### API Calling

To make an API call, the native `CURL` library was linked into the project to make a HTTP API call. This did require the archaic approach of setting the headers of the HTTP API call.

> Note: Responses from the StackOverflow API need to be dezipped as the response is gzipped to reduce bandwidth.

Making the call, a stringified JSON response is returned back of the questions that are relevant to the URL and request body (only appropriate to `POST` requests).

### API Response Handler

The stringified JSON response is passed to a response handler, specifically the method

    json_parse(char* json_as_string)
    
within the file `query/json_parser.c`.

Using the [`cJSON`](https://github.com/DaveGamble/cJSON) library, the stringified JSON response is converted into C structs to mimic the JSON structure and provide ease to reading the API response for the stages of the pipeline further down the line.

### Displaying

Using the parsed response returned from the API, the results can be formatted for our command line interface browser. This is all done in `display/`.

One thing that should be noted is that the body of a StackOverflow question is returned as a mix of HTML Markdown. We render this using Lynx to uphold the formatting of the question.