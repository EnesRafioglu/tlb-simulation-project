import csv
import random


def generate_uint32():
    value = random.randint(0, 2 ** 32 - 1)

    number_format = random.choice(['hex', 'dec', 'oct'])

    if number_format == 'hex':
        return f'0x{value:08x}'
    elif number_format == 'dec':
        return f'{value}'
    elif number_format == 'oct':
        return f'00{value:011o}'

    return value


def generate_uint32_32_bit_aligned():
    value = random.randint(0, 2 ** 32 - 1)
    value = value & 0xFFFFFFFC

    number_format = random.choice(['hex', 'dec', 'oct'])

    if number_format == 'hex':
        return f'0x{value:08x}'
    elif number_format == 'dec':
        return f'{value}'
    elif number_format == 'oct':
        return f'00{value:011o}'

    return value


def to_uint32(value):
    if value.startswith('0x'):
        return int(value, 16)
    elif value.startswith('0'):
        return int(value, 8)
    else:
        return int(value)


def file_to_array(filename):
    lines = []
    with open(filename, 'r') as file:
        reader = csv.reader(file)
        for row in reader:
            we, addr, data = row

            addr = to_uint32(addr)
            if we == 'W' or we == 'w':
                data = to_uint32(data)
                we = 1
            else:
                we = 0

            lines.append([we, addr, data])

    for line in lines:
        if line[0] == 0:
            print('{' + f'{line[0]}' + ', ' + f'{line[1]}' + ', ' + '0' + '},')
        else:
            print('{' + f'{line[0]}' + ', ' + f'{line[1]}' + ', ' + f'{line[2]}' + '},')

    return lines


def generate_file(n):
    lines = []
    filename = 'input.csv'
    for _ in range(n):
        we = random.choice(['W', 'R', 'w', 'r'])
        addr = generate_uint32_32_bit_aligned()

        if we == 'W' or we == 'w':
            data = generate_uint32()
            lines.append([we, addr, data])
        else:
            lines.append([we, addr, ""])

    with open(filename, 'w', newline='') as file:
        writer = csv.writer(file, delimiter=',', doublequote=True, quoting=csv.QUOTE_NONE)
        writer.writerows(lines)


def main():
    generate_file(1000)
    lines = file_to_array('input.csv')


if __name__ == '__main__':
    main()
