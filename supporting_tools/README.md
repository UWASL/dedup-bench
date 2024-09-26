# Manual for Config Generation Bash Script
This script is designed to automate the process of generating configuration files for a variety of chunking techniques.

## Usage
The general usage pattern is:

`./config_script.sh [OPTIONS]`

[OPTIONS] are optional parameters that allow you to customize the behavior of the script.

Options
The script recognizes the following options:

`-h --help`: help menu

`-s`: If this parameter is given, the script will operate in silent mode, producing no console output.

`--avg-size` SIZE`: This parameter allows you to specify the average block size. If not provided, it defaults to 8096.

`--min-size` SIZE : This parameter allows you to specify the minimum block size. If not provided, the script will calculate it based on the average size.

`--max-size` SIZE: This parameter allows you to specify the maximum block size. If not provided, the script will calculate it based on the average size.

`--techniques` TECHNIQUES: This parameter allows you to specify a comma-separated list of chunking techniques that the script should use. The accepted values are FIXED, RABINS, AE, GEAR, FASTCDC, RAM, CRC. If not provided, all techniques are used.

`-t` Time: This parameter allows you to specify a time or a suffex for the output files

## Behavior

Once the script has processed its parameters, it behaves as follows:

It creates directories config_[time or suffix] and hashes[time or suffix] if they do not already exist.

It generates a configuration file for each specified technique. These files are named in the format \<technique>_\<avg-size>.conf and are stored in the config directory. If a file with the same name already exists, it is overwritten.

## Error Handling

The script checks the validity of its input in several ways:

If a non-recognized option is given, it produces an error message and stops.

If the --techniques option is given but one or more of the specified techniques are not recognized, it produces an error message and stops.

By design, the script overwrites any existing files with the same names as those it needs to create.

# Manual for Deduplication Bash Script

This script is designed to automate the process of running the deduplication tool (dedup.exe) and (measure_dedup.exe) on a given directory and outputs the result in single file

The script also includes options for compressing all files in the given directory, operating in silent mode.

## Usage
The general usage pattern is:

`./dedup_script.sh [OPTIONS] <DIRECTORY>`

[OPTIONS] are optional parameters that allow you to customize the behavior of the script, and <DIRECTORY> is the path to the directory that you want the script to act on.

Options
The script recognizes the following options:

`-h --help`: help menu

`-c`: If this parameter is given, the script will compress all files in the provided directory using gzip before proceeding with the rest of its operations.

`-s`: If this parameter is given, the script will operate in silent mode, producing no console output.

`-t` Time: This parameter allows you to specify a time or a suffex for the output files

## Behavior

Once the script has processed its parameters, it behaves as follows:

If the -c option was given, it compresses all files in the given directory.

The script reads config directory ideally created with config_script and then executes `dedup.exe` and `./measure-dedup.exe` with each configuration file in turn, passing the directory and the configuration file as parameters. The output of each execution is appended to a file named results.txt, with a separator line and the name of the configuration file preceding the output.

## Error Handling

The script checks the validity of its input in several ways:

If a non-recognized option is given, it produces an error message and stops.

If the --techniques option is given but one or more of the specified techniques are not recognized, it produces an error message and stops.

If no directory is provided, it produces an error message and stops.

By design, the script overwrites any existing files with the same names as those it needs to create.


# Manual for Archives items extracting Script

## Description

The script processes a CSV file containing paths to archive files and their corresponding passwords (if applicable). For each archive file, it extracts the contents using `archive_ctl` and outputs the results to an `output_data` directory. The script also offers options to suppress informative messages and/or log messages and errors to a file.

## Usage

```
Usage: archive_extract.sh [-s] [--log <log_file>] <csv_file>
```

## Options

1. `-s`: When this option is used, the script runs in silent mode. It suppresses informative messages and only shows error messages.

2. `--log <log_file>`: This option redirects both informative messages and error messages to the specified log file. Replace `<log_file>` with the path to your desired log file. If this option is not used, messages are discarded and not stored.

3. `-h`, `--help`: This option displays the help menu, which includes the usage information and descriptions of options.

4. `-o`, `--output`: This option specifies the output directory path for extracted files. Defaults to ./output_data

5. `<csv_file>`: This is the path to the CSV file that the script should process. This argument is required.

## CSV File Structure

The CSV file should consist of two columns. The first column contains the paths to the archive files. The second column contains the passwords for the archive files. If an archive file does not have a password, "NP" (No Password) should be entered in the password column.

### Sample CSV

```
ip-172-31-90-168-CACF02FE-DC5E-48F3-A8CC-4FD5D24103A6-99CC3E37-FF23-4E9A-A867-D0F9C4017467A.tibx,NP
./ip-172-31-90-168-CACF02FE-DC5E-48F3-A8CC-4FD5D24103A6-99CC3E37-FF23-4E9A-A867-D0F9C4017467A.tibx,123456
~/boo/ip-172-31-90-168-CACF02FE-DC5E-48F3-A8CC-4FD5D24103A6-99CC3E37-FF23-4E9A-A867-D0F9C4017467A.tibx,password

```

## Steps of Operation

1. The script reads the CSV file and processes each line (i.e., each archive file) one by one.
2. if --output is not spacified, it creates an `./output_data` directory if it doesn't already exist. else it creates a new folder specified by --output.
3. The script runs the `archive_ctl` command on the archive file. If a password is required, it includes the password in the command.
4. The script parses the output of the `archive_ctl` command to retrieve the "items" field from the returned JSON object.
5. It then extracts each item (ignoring directories) from the archive file and saves it in the `output_data` directory or the directory specified by --output.
6. If an error occurs at any point (such as if the "items" field is not found in the output JSON), the script shows an error message and terminates.

Please note that this script is dependent on `jq` which is a tool to parse json objects. To install it follow these steps:

### Linux

1. **Ubuntu/Debian**

    ```bash
    sudo apt-get update -y
    sudo apt-get install -y jq
    ```

2. **Fedora**

    ```bash
    sudo dnf install jq
    ```

3. **CentOS/RHEL**

    ```bash
    sudo yum install epel-release
    ```

    Then, install `jq`:

    ```bash
    sudo yum install jq
    ```

### macOS

If you're using [Homebrew](https://brew.sh/), you can install `jq` by running:

```bash
brew install jq
```

Also make sure to run it in the same directory with archive_ctl tool
