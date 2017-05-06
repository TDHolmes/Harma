import gdb
import os


def execute_gdb_command(cmd_string, verbose=True):
    if verbose:
        print("(gdb) {}".format(cmd_string))
    gdb.execute(cmd_string)


def find_ports():
    ports = []
    dev_dir = os.listdir("/dev/")
    for thing in dev_dir:
        if "cu.usbmodem" in thing:
            ports.append(thing)

    return ports


def choose_port(list_of_ports):
    print("\nPorts:")
    for ind, port in enumerate(list_of_ports):
        print("\t{}: {}".format(ind, port))
    while True:
        try:
            choice = int(raw_input("Which port do you choose? "))
        except Exception:
            print("Invalid choice.")
            continue

        if choice < len(list_of_ports):
            return list_of_ports[choice]
        else:
            print("Invalid index.")


def choose_file(list_of_files):
    print("\nFiles:")
    for ind, f in enumerate(list_of_files):
        short_f = "/".join(f.split("/")[-3:])
        short_f = "(...)/" + short_f
        print("\t{}: {}".format(ind, short_f))
    while True:
        try:
            choice = int(raw_input("Which file do you choose? "))
        except Exception:
            print("Invalid choice.")
            continue

        if choice < len(list_of_files):
            return list_of_files[choice]
        else:
            print("Invalid index.")


def find_elf_files(directory):
    elf_files = []
    for root, dirs, files in os.walk(directory):
        for f in files:
            if ".elf" in f:
                filepath = os.path.join(root, f)
                print filepath
                elf_files.append(filepath)
    return elf_files


if __name__ == '__main__':
    port = choose_port(find_ports())
    f = choose_file(find_elf_files(os.path.abspath("../")))
    execute_gdb_command("target extended-remote /dev/{}".format(port))
    execute_gdb_command("monitor swdp_scan")
    execute_gdb_command("attach 1")
    execute_gdb_command("file {}".format(f))
    execute_gdb_command("load")
