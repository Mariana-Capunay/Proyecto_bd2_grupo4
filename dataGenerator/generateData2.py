import csv
from faker import Faker
import random

# Crea una instancia de Faker para generar datos ficticios
fake = Faker()

# Especifica la cantidad de registros que deseas generar
num_records = 1000000

# Define el nombre del archivo CSV
csv_filename = "dataset_2.csv"

# Define las columnas y longitudes fijas
columnas = [
    ("Código", 8),
    ("Nombre", 40),
    ("Stock", None),  # Usaremos None para datos de longitud fija
    ("Categoría", 25),
    ("Precio", None)  # Usaremos None para datos de longitud fija
]

# Abre el archivo CSV en modo escritura
with open(csv_filename, mode="w", newline="") as file:
    writer = csv.writer(file)

    # Escribe la fila de encabezados
    writer.writerow([col[0] for col in columnas])

    # Genera y escribe los datos ficticios en el archivo CSV
    for _ in range(num_records):
        codigo = fake.random_int(min=10000000, max=99999999)
        nombre_producto = fake.word().capitalize() + " " + fake.word().capitalize()
        stock = fake.random_int(min=0, max=1000)
        categoria = fake.word().capitalize()
        precio = round(random.uniform(10.0, 1000.0), 2)

        # Ajusta la longitud de las cadenas de texto
        codigo = str(codigo).rjust(8, "0")
        nombre_producto = nombre_producto[:40].ljust(40, " ")
        categoria = categoria[:25].ljust(25, " ")
        stock = str(stock)
        precio = str(precio)

        # Escribe la fila en el archivo CSV
        row = [codigo, nombre_producto, stock, categoria, precio]
        writer.writerow(row)

print(f"Se han generado {num_records} registros en el archivo CSV: {csv_filename}")
