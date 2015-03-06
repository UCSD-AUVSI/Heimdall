set(gocr_source_files
    amiga.h
    barcode.c
    barcode.h
    box.c
    database.c
    detect.c
    gocr.c
    gocr.h
    jconv.c
    job.c
    lines.c
    list.c
    list.h
    ocr0.c
    ocr0.h
    ocr0n.c
    ocr1.c
    ocr1.h
    otsu.c
    otsu.h
    output.c
    output.h
    pcx.c
    pcx.h
    pgm2asc.c
    pgm2asc.h
    pixel.c
    pnm.c
    pnm.h
    progress.c
    progress.h
    remove.c
    tga.c
    tga.h
    unicode.c
    unicode_defs.h
    unicode.h
    libPgm2asc.0.50.so
    config.h
    version.h
    )

add_library(gocr ${gocr_source_files})

target_link_libraries(gocr sharedutils)
