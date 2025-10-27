# SeqCDC Parameter Search Procedure

This directory contains the scripts needed to run the parameter search for SeqCDC. SeqCDC has three parameters _SeqLength_, _SkipTrigger_, and _SkipSize_. In addition, it has _OpMode_, _MinChunkSize_, and _MaxChunkSize_ . The parameter search helps determine the best combination of values for a given dataset. The details can be found in the paper.

## Steps to run parameter search

The following symbols are used in this section:
- **TBA**: Code release Pending

The steps to run the search are:

#### Monte Carlo Simulations to determine value limits

1. Generate random files for Monte-Carlo simulations with `generate_random_files.sh`. The script generates _K_ files of 1 GB each containing random data from `/dev/random/`.
2. **TBA**: Determine the limits for each parameter value for the chosen target average chunk size with `run_monte_carlo.py`.

#### Parameter Grid Search

3. Plug the limits and increments into `config_parameters.csv` by modifying it as necessary.
4. Generate configuration files compatible with DedupBench using `generate_param_search_configs.py`. Move them to the `build/` directory.
    ```
        ./generate_param_search_configs.py config_parameters.csv <config_dir_name>
    ```
5. Create a uniform sample of 10% of your dataset and store it in `<grid_search_dataset_dir>`.
6. Run the grid search using `dedup_script.sh` in the `build/` directory:
    ```
        ./dedup_script.sh -c <config_dir_name> <grid_search_dataset_dir>
    ```

#### Analyze results

7. Use `parse_dedup_script_results.py` to collect the results from all configuration files:
    ```
        ./parse_dedup_script_results.py ../build/results.txt parsed_results.csv
    ```
8. Identify the top 5 combinations using the results from parsed_results.csv and run them on the entire dataset to identify the best combination.

