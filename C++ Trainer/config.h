// Enum types

#define ENGLISH (0)
#define PERSIAN (1)

// START: Configurations

// Set to desired language to use the proper dataset: Could be ENGLISH or PERSIAN
#define TRAIN_LANG ENGLISH

// Training scheduling parameters. A formula similar to exponential decay will be used.

#define ENGLISH_PARAMETERS_SAVE_PATH "../JS Interactive/netdata-english.json.lzstring"
#define ENGLISH_PROCESS_EPOCHS 25
#define ENGLISH_STARTING_LEARNING_RATE 0.2
#define ENGLISH_ENDING_LEARNING_RATE 0.05

#define PERSIAN_PARAMETERS_SAVE_PATH "../JS Interactive/netdata-persian.json.lzstring"
#define PERSIAN_PROCESS_EPOCHS 25
#define PERSIAN_STARTING_LEARNING_RATE 0.2
#define PERSIAN_ENDING_LEARNING_RATE 0.05

#define EXP_DECAY_BASE 1.00001 // Close to one so we have a smoother decay. Larger values in the beginning and fine-tuning near the end seem to be crucial.

#define TEST_PERCENTAGE 10

#define SHOW_DEBUG true
#define SHOW_DEBUG_NTH_SAMPLE 2000 // Will show neural network's input/output, accuracy, loss every nth sample
#define CHECKPOINT_NTH_SAMPLE 10000 // Will save the neural network's paramters to a file every nth sample

// English language only: Determines whether the mnist characters should be randomly transformed (scale + random location)
// Useful only for the web version to ensure neural network flexibility of the handwritten digit's position.
// Transformation always applies to persian digits as input size is variable.
#define ENGLISH_DATASET_RANDOM_TRANSFORMATION true

// END: Configurations




#define DATASETS_PATH "datasets"

#if TRAIN_LANG == ENGLISH
#define LANG_STR "English"
#define PARAMETERS_SAVE_PATH ENGLISH_PARAMETERS_SAVE_PATH
#define PROCESS_EPOCHS ENGLISH_PROCESS_EPOCHS
#define STARTING_LEARNING_RATE ENGLISH_STARTING_LEARNING_RATE
#define ENDING_LEARNING_RATE ENGLISH_ENDING_LEARNING_RATE
#elif TRAIN_LANG == PERSIAN
#define LANG_STR "Persian"
#define PARAMETERS_SAVE_PATH PERSIAN_PARAMETERS_SAVE_PATH
#define PROCESS_EPOCHS PERSIAN_PROCESS_EPOCHS
#define STARTING_LEARNING_RATE PERSIAN_STARTING_LEARNING_RATE
#define ENDING_LEARNING_RATE PERSIAN_ENDING_LEARNING_RATE
#else
#error Undefined training language was selected
#endif