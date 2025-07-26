namespace RainforestControlTool.Independent.Helpers;

/// <summary>
/// Generates STM32 compatible CRCs
/// </summary>
public static class STM32CrcGenerator
{
    private const UInt32 InitialValue = 0xFFFFFFFF;

    private const UInt32 Polynomial = 0x04C11DB7;

    /// <summary>
    /// Calculate STM32-compatible CRC32
    /// </summary>
    public static UInt32 CalculateStmCrc32(byte[] data)
    {
        var resultBuffer = new List<UInt32>();
        var sizeIn32 = data.Length / 4;

        for (var i = 0; i < sizeIn32; i++)
        {
            var start = i * 4;
            var fourBytes = new byte[4];
            Array.Copy(data, start, fourBytes, 0, 4);
            
            resultBuffer.Add(BitConverter.ToUInt32(fourBytes, 0));
        }

        var remainingBytes = data.Length % 4;
        if (remainingBytes != 0)
        {
            // Constructing last uint from bytes and alignment zeroes
            var lastUint = new byte[] { 0, 0, 0, 0 };

            for (var i = 0; i < remainingBytes; i++)
            {
                lastUint[i] = data.ElementAt(sizeIn32 * 4 + i);
            }

            resultBuffer.Add(BitConverter.ToUInt32(lastUint, 0));
        }

        return CalculateStmCrc32(resultBuffer.ToArray());
    }
    
    /// <summary>
    /// STM-compatible CRC32 calculation (based on https://community.st.com/s/question/0D50X00009XkgnVSAR/crc32-calculation-in-c)
    /// </summary>
    private static UInt32 CalculateStmCrc32(UInt32[] data)
    {
        _ = data ?? throw new ArgumentNullException(nameof(data));
        
        UInt32 crc = InitialValue;
        foreach (var dataItem in data)
        {
            crc ^= dataItem;

            for (var bitIndex = 0; bitIndex < 32; bitIndex++)
            {
                if ((crc & 0x80000000) != 0)
                {
                    crc = ((crc << 1) ^ Polynomial);
                }
                else
                {
                    crc <<= 1;
                }
            }
        }
        
        return crc;
    }
}