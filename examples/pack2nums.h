int pack2nums()
{
    // Exemplos de números
    uint16_t num1 = 0b111111; // 6 bits (valor máximo: 63)
    uint16_t num2 = 0b1010;   // 4 bits (valor máximo: 15)
    uint16_t packed = 0;

    // Empacotar em 10 bits
    packed = PACK_2NUMSTO_XBITS(num1, num2, 6, 10);
    printf("Empacotado: 0x%X\n", packed);

    // Desempacotar
    uint16_t unpacked_num1 = 0, unpacked_num2 = 0;
    UNPACK_2NUMSFROM_XBITS(packed, unpacked_num1, unpacked_num2, 6, 10);
    printf("Desempacotado: num1 = %u, num2 = %u\n", unpacked_num1, unpacked_num2);
}
