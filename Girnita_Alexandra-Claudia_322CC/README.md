Tema 1 - Loader de Executabile
Nume: Girnita Alexandra-Claudia
Grupa: 322CC

Rezolvarea temei s-a bazat pe implementarea functiei segv_handler pentru tratarea evenimentelor de tip segmentation faul. De fiecare data cand aceasta este apelata trecem prin toate segmentele executabilului pentru a-l identifica pe cel la la nivelul caruia are loc segmentation fault. Urmatorul pas este calcularea paginii corespunzatoare din segment si verificarea maparii acesteia in memorie. In cazul paginilor nemapate, folosim functia mmap si functia read. Dimensiunea citita poate fi maxim egala cu dimensiunea paginii sau ce a ramas necitit din fisier. La final se seteaza permisiunile corespunzatoare segmentului. In orice alt caz se apeleaza handlerul default.