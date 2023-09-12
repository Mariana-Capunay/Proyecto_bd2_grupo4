import csv
from faker import Faker
import random

# Crea una instancia de Faker para generar datos ficticios
fake = Faker()

# Especifica la cantidad de registros que deseas generar
num_records = 1000000

# Define el nombre del archivo CSV
csv_filename = "dataset_1.csv"

# Define las columnas y longitudes fijas
columnas = [
    ("DNI", 8),
    ("Nombre Completo", 40),
    ("Edad", None),  # Usaremos None para datos de longitud fija
    ("Ciudad", 25),
    ("Sueldo", None)  # Usaremos None para datos de longitud fija
]

# Abre el archivo CSV en modo escritura
with open(csv_filename, mode="w", newline="") as file:
    writer = csv.writer(file)
    
    # Escribe la fila de encabezados
    writer.writerow([col[0] for col in columnas])
    
    # Genera y escribe los datos ficticios en el archivo CSV
    for _ in range(num_records):
        dni = fake.random_int(min=10000000, max=99999999)
        nombre_completo = fake.name()
        edad = fake.random_int(min=18, max=80)
        ciudad = fake.city()
        sueldo = round(random.uniform(1000.0, 5000.0), 2)

        # Ajusta la longitud de las cadenas de texto
        dni = str(dni).rjust(8, "0")
        nombre_completo = nombre_completo[:40].ljust(40, " ")
        ciudad = ciudad[:25].ljust(25, " ")
        edad = str(edad)
        sueldo = str(sueldo)

        # Escribe la fila en el archivo CSV
        row = [dni, nombre_completo, edad, ciudad, sueldo]
        writer.writerow(row)

print(f"Se han generado {num_records} registros en el archivo CSV: {csv_filename}")
