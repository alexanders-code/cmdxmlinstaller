A simple tool to create a installation packages!
Alpha version

Allows you to create self-extracting packages using commands from the command line or xml configuration file
Позволяет создавать самораспаковывающиеся пакеты с помощью команд из командной строки или конфигурационного xml файла

Command format - command "value"
For example - open "C:\sfx\sfxsample.xml"

A list of commands
open - to load the configuration xml file, parameters, the path to the xml file 
save - save the xml file generated configuration, parameters, the path to the xml file 
view - viewing the structure of the current installation package
test - the test the current installation package
build - to create the installation package
comment - the comment, parameters, the comment text
exit - the output from the program

A list of XML elements
can be entered from the command line or xml file
installer
may contain the following elements:
	name - the name of the installation package, parameters, the path or name of the installation package
	extract - the path to extract the files, parameters, the path to extract the files
	interface
	may contain the following elements:
		progrees - displays the installation process as a percentage or as a line, available values "percent" or "bar"
		color - sets the color of the installation process available values "red", "green", "blue" or any combination of the given colors separated by "|".
		for example color "green|red"

product
the data in this element is written to the resources of the installation package
may contain the following elements:
	name - the name of the installation package, parameters, the name of the installation package is not more than 50 characters
	version - the version of the installation package, parameters, the version of the installation package format "000.000.000.000"
	comapny - the company name of the installation package, parameters, the company name of the installation package is not more than 50 characters

file
the files that will be added to the installation
may contain the following elements:
	name - file name or mask to add to an existing installations, parameters, the path to file
	extract - the path to extract the files, parameters, the path to extract the files

Example xml installation project:

<installer>
<name>
"C:\sfx\one\sfxsample.exe"
</name>
<extract>
""
</extract>
<interface>
<progress>
"bar"
</progress>
<color>
"green | red"
</color>
</interface>
</installer>

<product>
<name>
"The name this program"
</name>
<version>
"00.00.00.01"
</version>
<company>
"Onecomm Ltd"
</company>
</product>

<file>
<name>
"C:\Settings\sfxclass\*.*"
</name>
<extract>
".\thisplace\"
</extract>
</file>

<file>
<name>
"c:\temp\jpg\*.jpg"
</name>
<!--"this is sample comment"
-->
</file>

Example command input:

installer name "C:\sfx\one\sfxsample.exe"
installer interface progress "percent" color "red"
probuct name "The name this program"
version "00.00.00.01"
comapny "Onecomm Ltd"
file name "C:\Settings\sfxclass\*.*"
extract ".\thisplace\"
file name "c:\temp\jpg\*.jpg"
comment "this is sample comment"
test
biuld
exit
