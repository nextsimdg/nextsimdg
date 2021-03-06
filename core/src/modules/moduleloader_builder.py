"""This module generates the inclusion .ipp files for the C++ ModuleLoader."""

def denamespace(nname):
    """Returns the last element of the name, without any of the qualifying
    namespaces."""
    return nname.split(":")[-1]

def get_pname(full_name):
    """Returns the name of the pointer to the stored instance of the
    implementing class, given the full name of that class."""
    return f"p_{denamespace(full_name)}"

def get_iname(full_name):
    """Returns the name of the stored implementation."""
    return f"i_{denamespace(full_name)}"

def get_pfname(full_name):
    """Returns the name of the function pointer for the interface, given its
    namespaced class name."""
    return f"pf_{denamespace(full_name)}"

def get_fname(impl):
    """Returns the function name for the implementation, given its namespaced string name."""
    return f"new{denamespace(impl)}"

def headers(all_implementations, ipp_prefix, hpp_prefix):
    """Generates the moduleLoaderHeaders.ipp file."""
    with open(f"{ipp_prefix}moduleLoaderHeaders.ipp", "w", encoding="utf-8") as fil:
        for interface in all_implementations:
            header_name = denamespace(interface["name"])
            fil.write(f"#include \"{hpp_prefix}{header_name}.hpp\"\n")
            for impl in interface["implementations"]:
                header_name = denamespace(impl)
                fil.write(f"#include \"{hpp_prefix}{header_name}.hpp\"\n")
            # An extra line between interfaces
            fil.write("\n")

def functions(all_implementations, ipp_prefix):
    """Generates the moduleLoaderFunctions.ipp file."""
    with open(f"{ipp_prefix}moduleLoaderFunctions.ipp", "w", encoding="utf-8") as fil:
        for interface in all_implementations:
            name = interface["name"]
            # Define the pointer to the stored implementation
            p_name = get_pname(name)
            fil.write(f"static {name}* {p_name};\n")
            # Define the function that returns the pointer to the stored implementation
            fil.write(
                "template<>\n"
                f"{name}& ModuleLoader::getImplementation()\n"
                "{\n"
                f"    return *{p_name};\n"
                "}\n"
                )
            # Define the pointer to function
            pf_name = get_pfname(name)
            fil.write(f"std::unique_ptr<{name}> (*{pf_name})();\n")
            # Define function that call the function pointer
            fil.write(
                "template<>\n"
                f"std::unique_ptr<{name}> ModuleLoader::getInstance() const\n"
                "{\n"
                f"    return (*{pf_name})();\n"
                "}\n"
                )
            for impl in interface["implementations"]:
                # The stored instance of the implementation
                fil.write(f"static {impl} {get_iname(impl)};\n")
                # The function that return the new unique_ptr for each implementation
                fil.write(
                    f"std::unique_ptr<{name}> {get_fname(impl)}()\n"
                    "{\n"
                    f"    return std::unique_ptr<{impl}>(new {impl});\n"
                    "}\n"
                    )
            # An extra line between interfaces
            fil.write("\n")

def names(all_implementations, ipp_prefix):
    """Generates the moduleLoaderNames.ipp file."""
    with open(f"{ipp_prefix}moduleLoaderNames.ipp", "w", encoding="utf-8") as fil:
        fil.write(
            "    m_availableImplementationNames = {\n"
            "        "
            )
        for interface in all_implementations:
            name = interface["name"]
            fil.write(
                "{\n"
                f"        \"{name}\", ""{\n"
                )
            for impl in interface["implementations"]:
                fil.write(f"            \"{impl}\",\n")
            fil.write(
                "            }\n"
                "        },"
                )
        fil.write("\n    };\n")

def assignments(all_implementations, ipp_prefix):
    """Generates the moduleLoaderAssignments.ipp file."""
    with open(f"{ipp_prefix}moduleLoaderAssignments.ipp", "w", encoding="utf-8") as fil:
        fil.write("        ")
        for interface in all_implementations:
            name = interface["name"]
            fil.write(
                f"if (module == \"{name}\") ""{\n"
                "            "
                )
            p_name = get_pname(name)
            pf_name = get_pfname(name)
            for impl in interface["implementations"]:
                fil.write(
                    f"if (impl == \"{impl}\") ""{\n"
                    f"                {p_name} = &{get_iname(impl)};\n"
                    f"                {pf_name} = &{get_fname(impl)};\n"
                    "            } else "
                    )
            fil.write(
                "{\n"
                "                throwup(module, impl);\n"
                "            }\n"
                "\n"
                "        } else "
                )
        fil.write("{ }")

def generate(all_implementations, ipp_prefix = '', hpp_prefix = ''):
    """Generates the .ipp inclusion files for ModuleLoader.cpp

    :param all_implementations: The vector of dictionaries that defines the
            interfaces and implementations thereof.
    :param ipp_prefix: A directory and file prefix that precedes the ipp file
            names to provide a path from the current working directory.
    :param hpp_prefix: A text directory and file prefix to add to the hpp file
            names to suit the locations in the build system.
    """
    headers(all_implementations, ipp_prefix, hpp_prefix)
    functions(all_implementations, ipp_prefix)
    names(all_implementations, ipp_prefix)
    assignments(all_implementations, ipp_prefix)

if __name__ == "__main__":

    import argparse
    import json

    parser =  argparse.ArgumentParser(description = "Build inclusion files for ModuleLoader.cpp")
    FILE_HELP = "JSON file containing the specification of interfaces and implementations"
    parser.add_argument("json_file", metavar = "files", nargs = '*', default = None,
                         type = argparse.FileType('r', encoding="utf-8"),
                         help = FILE_HELP)
    parser.add_argument("--ipp", dest = "ipp_prefix", default = "./",
                        help = "Path and file prefix to be added to the .ipp file names.")
    parser.add_argument("--hpp", dest = "hpp_prefix", default = "include/",
                        help = "Path to the module header file name.")
    args = parser.parse_args()

    DFILE = "modules.json"
    if (len(args.json_file) == 0):
        jsons = [DFILE]
    else:
        jsons = args.json_file

    alli =[]
    for jj in jsons:
        alli += json.load(jj)

    generate(alli, hpp_prefix = args.hpp_prefix, ipp_prefix = args.ipp_prefix)
