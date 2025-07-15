# Security Policy

## 📅 Supported Versions

We aim to support the latest stable release of C* (`main` branch). Older versions may be patched if widely used, but only on a case-by-case basis.

| Version | Supported |
|---------|-----------|
| main    | ✅ Yes     |
| older   | ⚠️ Best-effort |

## 🐞 Reporting a Vulnerability

If you discover a memory safety issue or vulnerability:

1. **Do not open a public issue**.
2. Email the maintainer directly: `rashid.vali@<yourdomain>.com` (or use GitHub's private report feature if enabled).
3. Provide:
   - Clear description of the issue
   - Code snippet or reproduction steps
   - Affected version or commit hash

We take security seriously and will respond within **3 business days**.

## 🔐 Mitigation Philosophy

C* is designed for:
- Embedded & critical systems
- Legacy modernization
- Environments where memory safety is paramount

We encourage all users to enable `SAFE_LOG` diagnostics during development and testing.

## 🧪 Auditing

Security audits (manual or automated) are welcome. If you are from a security research institution and would like to contribute, please [open an issue](../../issues) or reach out via email.

---

C* — Memory Safety, Built-In.
