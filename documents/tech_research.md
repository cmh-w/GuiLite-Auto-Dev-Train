# Tech Research Survey (2025–2026)

> Compiled: 2026-07-28 | Scope: AI-assisted development tools, MCP protocol, AI software prototype design, GitHub automation, DevOps pipelines

---

## Table of Contents

1. [AI-Assisted Development Tools Comparison](#1-ai-assisted-development-tools-comparison)
2. [Model Context Protocol (MCP)](#2-model-context-protocol-mcp)
3. [AI Software Prototype Design](#3-ai-software-prototype-design)
4. [GitHub Automation & Open-Source Tools](#4-github-automation--open-source-tools)
5. [DevOps Pipeline Technologies](#5-devops-pipeline-technologies)
6. [Summary](#6-summary)

---

## 1. AI-Assisted Development Tools Comparison

### Overview

Three major paradigms have emerged in AI-assisted coding as of mid-2026: AI-first IDEs, platform-integrated assistants, and terminal-native agents.

### Tool Comparison

| Feature | Cursor | GitHub Copilot | Claude Code |
|---|---|---|---|
| **Type** | AI-first IDE (VS Code fork) | Editor extension / platform | Terminal agent (CLI) |
| **Pricing** | $20/m Pro | $10/m Individual | $100/m Pro (free tier available) |
| **Context Window** | ~10k lines (rule-based) | Full repo (VS Code) / partial (JetBrains) | 200K token context |
| **Tab Completion** | Yes (Multi-line) | Yes (multi-line, agent mode) | No (generation only) |
| **Agent Mode** | Yes (Composer) | Yes (Copilot Agent) | Native (agent-first) |
| **Multi-File Edit** | Yes | Yes | Yes |
| **Terminal Integration** | Via chat | Via agent mode | Native (built-in tool execution) |
| **Model Choice** | Multiple (GPT-4o, Claude, etc.) | GPT-4o (primary) | Claude Opus 4.5+ |
| **Key Strength** | Proactive diff, fast tab completion | Platform-embedded, no context switch | Deep reasoning, large context, tool use |
| **Key Weakness** | Fork-based (upstream lag) | Quality ceiling on complex tasks | Terminal-only (no editor UI) |

### Paradigm Analysis

- **Cursor** refines the editor experience with intelligent tab completion, proactive diff suggestions, and inline code generation. Suits developers who want AI deeply embedded in the editing workflow.
- **Copilot** leverages GitHub platform data (public repos, issues, PRs) for contextually relevant suggestions. Best for teams already in the Microsoft ecosystem.
- **Claude Code** operates as a terminal-based agent with full file system and shell access. Excels at complex multi-step reasoning, refactoring, and research tasks where deep context is needed.

**Market trend (2026):** The lines are blurring — Cursor adds agent features, Copilot adds deeper agent mode, Claude Code is exploring editor integrations. The "agent vs. copilot vs. pair programmer" taxonomy is converging toward unified AI development agents.

### Sources
- SitePoint — "Cursor vs Copilot vs Claude Code: Which AI Coding Tool Wins?" (2026-03-20)
- Cursor pricing page (cursor.com)
- GitHub Copilot documentation (docs.github.com/copilot)

---

## 2. Model Context Protocol (MCP)

### Overview

The Model Context Protocol (MCP), originally launched by Anthropic in late 2024, has become an open standard for connecting AI assistants to tools and data sources. Version 2026-07-28 is a major release candidate with breaking changes.

### 2026-07-28 Release Candidate — Key Changes

| Change | Description | Impact |
|---|---|---|
| **Stateless HTTP Core** | Removed JSON-RPC transport; moved to standard HTTP with SSE for streaming | Dropped persistent connections; every request is independent |
| **Authentication** | OAuth 2.0 / OpenID Connect alignment | Servers can enforce auth; clients carry access tokens |
| **Server-Discovered Capabilities** | Server advertises capabilities (tools, resources, prompts) dynamically | No more static config; clients discover at runtime |
| **New URL Scheme** | `http://` / `https://` instead of `mcp://` | Standard web infrastructure; firewalls/proxies work naturally |
| **MCP Apps** | Lightweight clients for running MCP servers locally or remotely | Enables consumer-grade MCP usage without CLI |
| **MCP Tasks** | Structured long-running task execution framework | Background jobs, multi-step workflows, status polling |
| **Handshake Removed** | No separate `initialize`/`initialized` phase | Connection phases simplified; capabilities exchanged inline |
| **Transport Headers** | `mcp-version`, `mcp-auth`, `mcp-request-id` | Standard HTTP headers for MCP-specific metadata |
| **Deprecation Policy** | Explicit deprecation headers; minimum 3-month migration window | Old protocol versions will be supported for transition |

### Architecture

```
┌─────────────────┐       HTTP/HTTPS        ┌──────────────────┐
│   MCP Client     │ ◄─────────────────────► │   MCP Server      │
│  (App/Task/CLI)  │    SSE for streaming    │ (Tools/Resources) │
└─────────────────┘                          └──────────────────┘
       │                                              │
       │ OAuth 2.0                                    │
       ▼                                              ▼
┌─────────────────┐                          ┌──────────────────┐
│   Auth Provider  │                          │   Data Sources    │
│ (IdP / OIDC)     │                          │ (DBs, APIs, FS)   │
└─────────────────┘                          └──────────────────┘
```

### Key Implications

- **Breaking change from all prior versions** — existing MCP clients/servers need migration
- **Standard HTTP** enables reuse of existing web infrastructure (load balancers, CDNs, API gateways)
- **OAuth/OIDC** aligns MCP with enterprise security requirements
- **MCP Apps/Tasks** expand the protocol beyond developer tooling toward general AI agent platforms
- **Deprecation policy** provides a structured migration path

### Sources
- MCP Specification Blog — "The next chapter of MCP" (2026-07-28)
- GitHub — modelcontextprotocol/specification

---

## 3. AI Software Prototype Design

### Overview

AI-powered prototype-to-code tools have proliferated since 2024. The most active open-source project in this space as of July 2026 is **open-codesign**.

### Open-Codesign

| Attribute | Detail |
|---|---|
| **Project** | OpenCoworkAI/open-codesign |
| **Stars** | ~6,900 |
| **License** | MIT |
| **Stack** | Electron + React |
| **Model Support** | Multi-model BYOK (bring your own key) — Claude, GPT-4o, Gemini, open-source models |
| **Input** | Screenshots, wireframes, text prompts |
| **Output** | Production-ready frontend code (React, Vue, etc.) |
| **Notable** | Active development (commits within days); community plugins |

### Commercial Alternatives

| Tool | Pricing Model | Strengths | Weaknesses |
|---|---|---|---|
| **v0 by Vercel** | Freemium | Tight Next.js/Shadcn integration | Vercel lock-in |
| **Lovable** | Subscription | Full-stack generation | Proprietary |
| **Claude Design** (Anthropic) | Bundled with Claude Pro | Strong reasoning + generation | Limited customization |
| **Screenshot-to-Code** | Open source (OSS) | Simple, focused | Limited to screenshots |

### Architecture Diagram (open-codesign)

```
User Input ──► Screenshot/Wireframe/Text
                    │
                    ▼
          ┌─────────────────┐
          │  Model Router    │
          │ (BYOK: Claude,   │
          │  GPT-4o, Gemini) │
          └────────┬────────┘
                   │
                   ▼
          ┌─────────────────┐
          │  Layout Analyzer │
          │ (Component tree) │
          └────────┬────────┘
                   │
                   ▼
          ┌─────────────────┐
          │  Code Generator  │
          │ (React/Vue/HTML) │
          └────────┬────────┘
                   │
                   ▼
          ┌─────────────────┐
          │  Live Preview    │
          │ (Electron shell) │
          └─────────────────┘
```

### Key Trend (2026)

AI prototyping tools are shifting from "generate a single page" toward **multi-page app generation** with routing, state management, and backend integration. The open-source ecosystem (led by open-codesign) is closing the gap with commercial tools.

### Sources
- GitHub — OpenCoworkAI/open-codesign
- Vercel v0 documentation (v0.dev)
- Lovable.dev product page

---

## 4. GitHub Automation & Open-Source Tools

### Overview

GitHub Actions has become the de facto CI/CD platform, with 20,000+ actions in the marketplace and deep platform integration (Dependabot, CodeQL, Copilot).

### Key Platform Features (2026)

- **Multi-OS / Multi-Arch**: Linux, Windows, macOS, ARM, GPU runners
- **Reusable Workflows**: Centralized workflow definitions across repos
- **Composite Actions**: Custom action composition without Docker
- **Dependabot Integration**: Auto-PR for dependency updates
- **Copilot for PRs**: AI-generated PR descriptions, code review summaries
- **Artifact Sharing**: Cross-job/ cross-workflow artifact passing

### Open-Source Tools & Ecosystem

| Tool | Purpose | Notes |
|---|---|---|
| **Act** (nektos/act) | Run Actions locally | 55k+ stars; essential for offline development |
| **Dagger** | CI/CD engine (container-native) | Portable across GitHub Actions, GitLab CI, local |
| **Terraform GitHub Provider** | Infrastructure-as-Code for GitHub resources | Manage repos, teams, Actions secrets declaratively |
| **Labeler** (actions/labeler) | Auto-label PRs | Community-maintained |
| **Stale** (actions/stale) | Close stale issues/PRs | Official GitHub action |
| **Upload/Download Artifact** | Binary artifact management | Official GitHub actions |

### Common Workflow Patterns

```
1. CI Pipeline
   ┌──────┐   ┌──────────┐   ┌─────────┐   ┌──────────┐
   │ Lint  │──►│   Test   │──►│  Build   │──►│  Publish  │
   └──────┘   └──────────┘   └─────────┘   └──────────┘

2. PR Automation
   ┌──────┐   ┌──────────┐   ┌─────────┐
   │ Label │──►│  Assign   │──►│  Auto-   │
   │       │   │ Reviewer  │   │  Merge   │
   └──────┘   └──────────┘   └─────────┘

3. Release Pipeline
   ┌────────┐   ┌──────────┐   ┌──────────┐
   │ Version │──►│   Build   │──►│  Publish  │──►│ Release  │
   │ Bump   │   │ + Sign   │   │ to npm   │   │  Notes   │
   └────────┘   └──────────┘   └──────────┘
```

### Sources
- GitHub Actions documentation (docs.github.com/actions)
- GitHub Marketplace (github.com/marketplace/actions)
- nektos/act GitHub repository

---

## 5. DevOps Pipeline Technologies

### Overview

DevOps in 2026 is being redefined from "automation" toward "intelligent delivery" — pipelines that self-scale, self-heal, and make autonomous decisions based on observability data.

### Key Trends

| Trend | Description | Maturity |
|---|---|---|
| **Self-Scaling Pipelines** | Dynamic resource scaling based on workload (more runners for big builds, fewer for small) | Emerging |
| **AI-Assisted Incident Response** | Pipelines that detect anomalies, suggest/apply rollbacks automatically | Early adoption |
| **Closed-Loop Observability** | Observability data fed back into pipeline decisions (auto-retry, canary promotion) | Growing |
| **Shift from "Tool Zoo"** | Consolidation of many point tools into unified platforms | Active |
| **Platform Engineering** | Internal developer platforms (IDPs) as product | Mainstream |
| **GitOps 2.0** | Declarative + AI-driven drift correction | Emerging |

### Tool Ecosystem Map

```
┌─────────────────────────────────────────────────────┐
│                   Pipeline Layer                      │
│  GitHub Actions │ GitLab CI │ Jenkins X │ Dagger     │
├─────────────────────────────────────────────────────┤
│                 Container / Runtime                   │
│  Docker │ Podman │ Kubernetes │ Nomad                │
├─────────────────────────────────────────────────────┤
│                  Observability                        │
│  OpenTelemetry │ Grafana │ Datadog │ Honeycomb       │
├─────────────────────────────────────────────────────┤
│                Security / Compliance                  │
│  CodeQL │ Trivy │ OPA │ Sigstore │ SLSA             │
├─────────────────────────────────────────────────────┤
│                 AI / Intelligence                     │
│  Copilot for PRs │ MCP │ LLM Ops │ AI agents         │
└─────────────────────────────────────────────────────┘
```

### Pipeline Evolution Timeline

```
2023 ─── Static CI/CD (YAML-driven, human-triggered)
  │
  ▼
2024 ─── Reusable workflows, composite actions
  │
  ▼
2025 ─── AI-assisted (Copilot in PRs, auto-suggestions)
  │
  ▼
2026 ─── Intelligent delivery (self-scaling, closed-loop observability,
         AI agents in pipeline decisions)
```

### Architecture: AI-Augmented Pipeline

```
                    ┌──────────────────┐
                    │   AI Agent / MCP  │
                    │  (pipeline brain) │
                    └────────┬─────────┘
                             │
  Code Push ──► ┌────────────┴────────────┐
                │    Pipeline Orchestrator │
                │  (self-scaling runners)  │
                └────────────┬────────────┘
                             │
        ┌────────────────────┼────────────────────┐
        │                    │                    │
        ▼                    ▼                    ▼
  ┌──────────┐       ┌──────────┐        ┌──────────┐
  │   Build   │       │   Test    │        │  Deploy   │
  │ (cache-   │       │ (parallel │        │ (canary / │
  │  aware)   │       │  sharding)│        │  rolling) │
  └──────────┘       └──────────┘        └─────┬────┘
                                                │
                                        ┌───────▼───────┐
                                        │  Observability │
                                        │  (feedback to  │
                                        │   AI agent)    │
                                        └───────────────┘
```

### Sources
- DevOpsDigest — "DevOps Trends to Watch in 2026" (2026-07-21)
- Dagger.io documentation
- OpenTelemetry project status (opentelemetry.io)

---

## 6. Summary

### Cross-Cutting Themes

1. **AI Agents are eating the toolchain** — From coding (Claude Code, Cursor) to pipeline orchestration (MCP agents) to prototyping (open-codesign), AI agents are becoming the universal interface.
2. **Convergence of open standards** — MCP moving to HTTP/OAuth aligns AI tool connectivity with standard web infrastructure, making it easier for enterprises to adopt.
3. **Open-source catching up to commercial** — open-codesign (6.9k stars) and Act (55k+ stars) demonstrate that OSS alternatives in the AI-dev tool space are viable and growing.
4. **DevOps becoming "Intelligent Delivery"** — Static CI/CD pipelines are evolving into autonomous, self-optimizing systems with closed-loop observability feedback.
5. **BYOK and multi-model** — Users increasingly expect to bring their own API keys and choose models, driving BYOK architectures in both code generation and agent tools.

### Technology Radar

| Technology | Stage | Recommendation |
|---|---|---|
| MCP (HTTP/OAuth) | Emerging (RC) | Watch — migrate when stable |
| Cursor / Copilot | Mainstream | Adopt for daily development |
| Claude Code (agent) | Growing | Adopt for complex research/refactoring |
| open-codesign | Growing | Evaluate for prototyping workflows |
| Act (local Actions) | Mainstream | Adopt for CI debugging |
| Dagger | Growing | Evaluate for portable CI |
| AI-augmented pipelines | Emerging | Pilot for observability feedback loops |
