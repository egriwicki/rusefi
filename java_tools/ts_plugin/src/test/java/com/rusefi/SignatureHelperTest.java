package com.rusefi;

import com.rusefi.core.SignatureHelper;
import org.junit.jupiter.api.Test;

import static org.junit.Assert.assertEquals;

public class SignatureHelperTest {
    @Test
    public void test() {
        String url = SignatureHelper.getUrl("rusEFI master.2020.07.06.frankenso_na6.2468827536").first;
        assertEquals("https://rusefi.com/online/ini/rusefi/master/2020/07/06/frankenso_na6/2468827536.ini", url);
    }
}
