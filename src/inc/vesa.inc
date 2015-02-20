; minos VESA info

Mode_Info:              
ModeInfo_ModeAttributes         rw      1
ModeInfo_WinAAttributes         rb      1
ModeInfo_WinBAttributes         rb      1
ModeInfo_WinGranularity         rw      1
ModeInfo_WinSize                rw      1
ModeInfo_WinASegment            rw      1
ModeInfo_WinBSegment            rw      1
ModeInfo_WinFuncPtr             rd      1
ModeInfo_BytesPerScanLine       rw      1
ModeInfo_XResolution            rw      1
ModeInfo_YResolution            rw      1
ModeInfo_XCharSize              rb      1
ModeInfo_YCharSize              rb      1
ModeInfo_NumberOfPlanes         rb      1
ModeInfo_BitsPerPixel           rb      1
ModeInfo_NumberOfBanks          rb      1
ModeInfo_MemoryModel            rb      1
ModeInfo_BankSize               rb      1
ModeInfo_NumberOfImagePages     rb      1
ModeInfo_Reserved_page          rb      1
ModeInfo_RedMaskSize            rb      1
ModeInfo_RedMaskPos             rb      1
ModeInfo_GreenMaskSize          rb      1
ModeInfo_GreenMaskPos           rb      1
ModeInfo_BlueMaskSize           rb      1
ModeInfo_BlueMaskPos            rb      1
ModeInfo_ReservedMaskSize       rb      1
ModeInfo_ReservedMaskPos        rb      1
ModeInfo_DirectColorModeInfo    rb      1
; VBE 2.0 extensions
ModeInfo_PhysBasePtr            rd      1
ModeInfo_OffScreenMemOffset     rd      1
ModeInfo_OffScreenMemSize       rw      1 